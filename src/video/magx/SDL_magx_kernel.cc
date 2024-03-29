////////////////////////////////////////////////////////////////////////
//////////////////////////  IPU + FB  //////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// Motorola IPU and FB init and configuration for SDL
//		by Anton Prozorov <prozanton@gmail.com>
//		(C) 2011
//
// Based on:
//		video driver whith IPU for MPlayer 
//		by Alexey Kuznetsov <kuznet@ms2.inr.ac.ru>   
//		(C) 2008
//

#include <linux/fb.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "ipu.h"
#include "ipu_alloc.h"
#include "mxcfb.h"
#include "mxc_pp.h"

#include "SDL_magx_kernel.h"

//Macros
#define bppToPixelSize(a) ((((a)-1)/8)+1)

//Hardware info
#define hw_dbpp (fb_orig_vinfo.bits_per_pixel)
#define hw_pixel_size bppToPixelSize(hw_dbpp)
#define p_height (fb_orig_vinfo.yres)
#define p_width (fb_orig_vinfo.xres)

//In info
#define in_pixel_size bppToPixelSize(in_dbpp)
int in_height;
int in_width;

//Out info
#define vo_dbpp (fb_vinfo.bits_per_pixel)
#define vo_pixel_size bppToPixelSize(vo_dbpp)

//IPU (Post-Processor)
static pp_start_params pp_st;
static char * pp_dma_buffer = NULL;
static unsigned int pp_dma_buffer_addr = 0;
pp_buf pp_desc;
static pp_init_params pp_init;

//FB
static char * pp_frame_buffer = NULL;

//FB&IPU id
int fb_dev_fd;
int fd_pp;

//Mouse
int cursor_x;
int cursor_y;
bool bShowCursor, bCanUseCursor;

//Init flag
int vo_init=0;
#define VO_INIT_FB			0x00000001
#define VO_INIT_IPU			0x00000002
#define VO_CONF_IPU			0x00000004
#define VO_ALLOC_DMA_MEM	0x00000008
#define VO_INIT_DB			0x00000010
#define VO_ALLOC_IPU_BUF	0x00000020
#define VO_INIT_OVERLAY		0x00000040

//FB info structure
struct fb_var_screeninfo fb_orig_vinfo;
struct fb_var_screeninfo fb_vinfo;
struct fb_fix_screeninfo fb_finfo;

//In info
int in_dbpp;

#if 0
#define DebugFunction() printf("MAGX_VO: kernel - %s()\n",__FUNCTION__) 
#else
#define DebugFunction()
#endif

void signal_handler(int sig) 
{
	perror("MyExept\n");
	if ( vo_init )
		uninit(); 
}

void preinit()
{
	DebugFunction();
	
	static int setSignal=1;
	if ( setSignal )
	{
		atexit(uninit);
		signal(SIGTERM, signal_handler);
		signal(SIGKILL, signal_handler);
		signal(SIGQUIT, signal_handler);
		signal(SIGILL,  signal_handler);
		setSignal = 0;
	}
	vo_init = 0;
	fb_dev_fd=fd_pp=0;
	pp_dma_buffer=0;
	memset(&pp_desc, 0, sizeof(pp_desc));
	memset(&pp_init, 0, sizeof(pp_init));
	in_height = 0;
	in_width = 0;
	cursor_x=0;
	cursor_y=0;
	bShowCursor=false;
	bCanUseCursor=false;
}

//Open and preinicialization FB
int initFB()
{
	DebugFunction();
	
	if ((fb_dev_fd = open("/dev/fb/0", O_RDWR)) == -1) 
	{
		printf("MAGX_VO: Can't open /dev/fb/0: %s\n", strerror(errno));
		return 0;
	}

	if (ioctl(fb_dev_fd, FBIOGET_VSCREENINFO, &fb_vinfo)) 
	{
		printf("MAGX_VO: Can't get VSCREENINFO: %s\n", strerror(errno));
		goto err_out_fd;
	}
	fb_orig_vinfo = fb_vinfo;

	if (ioctl(fb_dev_fd, FBIOGET_FSCREENINFO, &fb_finfo)) 
	{
		printf("MAGX_VO: Can't get FSCREENINFO: %s\n", strerror(errno));
		return 1;
	}

	printf("MAGX_VO: FB %u@%p\n", fb_finfo.smem_len, (void*)fb_finfo.smem_start);
	
	if ((pp_frame_buffer = (char*)mmap(0, p_width*p_height*hw_pixel_size, PROT_READ | PROT_WRITE,
				 MAP_SHARED, fb_dev_fd, 0)) == MAP_FAILED) 
	{
		printf( "MAGX_VO: Can't mmap %s\n", strerror(errno));
		return 0;
	}
	
	memset(pp_frame_buffer, 0, p_width*p_height*hw_pixel_size);
	
	vo_init|=VO_INIT_FB;
	return 1;
	
err_out_fd:
	close(fb_dev_fd);
	fb_dev_fd = -1;
	return 0;	
}

int initIPU()
{
	DebugFunction();
	
	if ((fd_pp = open("/dev/alt_mxc_ipu_pp", O_RDWR, 0)) < 0) 
	{
		perror("MAGX_VO: open(/dev/alt_mxc_ipu_pp)");
		
		printf("MAGX_VO: Try open /dev/mxc_ipu_pp...\n");
		if ((fd_pp = open("/dev/mxc_ipu_pp", O_RDWR, 0)) < 0) 
		{
			perror("MAGX_VO: open(/dev/mxc_ipu_pp)\n");
			return 0;
		}
	}
	
	vo_init|=VO_INIT_IPU;
	return 1;		
}

int setBppFB( uint32_t in_bpp )
{
	DebugFunction();
	
	if ( !(vo_init&VO_INIT_FB) )
		return 0;
	
	printf("MAGX_VO: set new bpp %d (%d) on FB\n", in_bpp, fb_vinfo.bits_per_pixel);

	if ( in_bpp != fb_vinfo.bits_per_pixel )
	{
		printf("MAGX_VO: switching to bpp: %d\n", in_bpp);
		fb_vinfo.bits_per_pixel = in_bpp;
		
		if (ioctl(fb_dev_fd, FBIOPUT_VSCREENINFO, &fb_vinfo)) 
		{
			printf("MAGX_VO: Can't set VSCREENINFO: %s\n", strerror(errno));
			return 0;
		}
		if (ioctl(fb_dev_fd, FBIOGET_VSCREENINFO, &fb_vinfo)) 
		{
			printf("MAGX_VO: Can't get VSCREENINFO: %s\n", strerror(errno));
			return 0;
		}
		if ( in_bpp != fb_vinfo.bits_per_pixel )
		{
			printf("MAGX_VO: ERROR switching to bpp: %d\n", in_bpp);
			return 0;
		}
	}
	
	printf("MAGX_VO: seted bpp on FB\n");

	return 1;
}

inline unsigned long int bppTopixfmt(int bpp, bool out)
{
	switch ( vo_dbpp )
	{
		case 16:
			return IPU_PIX_FMT_RGB565;
		case 32:
			return IPU_PIX_FMT_BGR32;			
		case 24:
		default:
			if ( out )
				return IPU_PIX_FMT_BGRA6666;
			else
				return IPU_PIX_FMT_RGB24;						
	}	
}

int configureIPU( uint32_t width, uint32_t height, uint32_t in_bpp, uint16_t in_rot )
{
	DebugFunction();
	
	if ( !(vo_init&VO_INIT_IPU) )
	{
		printf("MAGX_VO: before configure need init IPU!\n");
		return 0;
	}
	
	in_dbpp = in_bpp;
	in_height = height;
	in_width = width;
	
	int rot = 0;
	int pp_dma_count=0;	
	
	/* width must be aligned to 8, IPU does not accept unaligned
	 * values. This is not required for height, however I observed
	 * some strange solor artfacts when it is not aligned.
	 */
	int vwidth, vheight;
	vwidth = (width + 7) & ~7;
	vheight = (height + 7) & ~7;
	
	printf("MAGX_VO: vwidth=%d vheight=%d hw_pixel_size=%d\n",
		   vwidth, vheight, vo_pixel_size);
	
	if ( (vwidth < vheight)!=(p_width < p_height) ) 
	{
		pp_dma_count++;
		rot = in_rot;
		printf("MAGX_VO: screen rotated\n");
	}
	
	if ( (vo_dbpp!=in_dbpp) || 
			(!rot && (width>p_width || height>p_height) ) ||
			( rot && (width>p_height || height>p_width) )     
					)
	{
		pp_dma_count++;
		printf("MAGX_VO: screen scalling or bpp convert\n");
	}
	
	memset(&pp_init, 0, sizeof(pp_init));
	
	if ( pp_dma_count>1 )
		pp_init.mode = PP_PP_ROT;
	else if ( pp_dma_count==1 )
	{
		if ( rot )
			pp_init.mode = PP_ROT;
		else
			pp_init.mode = PP_PP;
	} else
		pp_init.mode = PP_DISABLE_ALL;
	
	printf("MAGX_VO: PP mode: %d, PP buf count: %d\n", pp_init.mode, pp_dma_count);
	
	pp_init.in_width = vwidth;
	pp_init.in_height = vheight;
	pp_init.in_stride = vwidth;
	pp_init.in_pixel_fmt = bppTopixfmt(in_dbpp,0);
	pp_init.out_width  = p_width;
	pp_init.out_height = p_height;
	pp_init.out_stride = p_width;
	pp_init.out_pixel_fmt = bppTopixfmt(in_dbpp,1);
	pp_init.mid_stride = p_height;
	pp_init.rot = rot;

	if (ioctl(fd_pp, PP_IOCTL_INIT, &pp_init) < 0) 
	{
		perror("MAGX_VO: PP_IOCTL_INIT");
		return 0;
	}
	
	memset(&pp_st, 0, sizeof(pp_st));
	if ( pp_dma_count>1 )
	{
		pp_st.mid.index = 0;
		pp_st.mid.size = IPU_MEM_ALIGN(p_width*p_height*vo_pixel_size);
		pp_st.mid.addr = ipu_malloc( pp_st.mid.size );
	}
	if ( pp_dma_count>0 )
	{
		pp_st.out.index = -1;
		pp_st.out.size = IPU_MEM_ALIGN(p_width*p_height*vo_pixel_size);
		pp_st.out.addr = fb_finfo.smem_start;	
		
		pp_st.in.index = -1;
		pp_st.in.size = IPU_MEM_ALIGN(vwidth*vheight*in_pixel_size);
		//Try locate input buffer in video memory
		if ( pp_st.in.size <= fb_finfo.smem_len-pp_st.out.size )
		{
			printf("MAGX_VO: Input buffer located in video memory!\n");
			pp_st.in.addr = fb_finfo.smem_start+pp_st.out.size;
			pp_dma_buffer = (char*)mmap (NULL, pp_st.in.size, 
							PROT_READ | PROT_WRITE, MAP_SHARED, 
								fb_dev_fd, pp_st.in.addr);	
		} else
		{
			pp_st.in.addr = ipu_malloc(pp_st.in.size);
			pp_dma_buffer = (char*)mmap (NULL, pp_st.in.size, 
							PROT_READ | PROT_WRITE, MAP_SHARED, 
								fd_pp, pp_st.in.addr);			
		}

		if ( pp_st.in.addr==NULL )
		{
			printf("MAGX_VO: Not enough memory to alloce the input buffer!\n");
			return 0;
		}

		vo_init|=VO_ALLOC_IPU_BUF;
	} else
	{
		pp_dma_buffer = NULL;
	}
	
	return 1;
}

int getAllDMAMem()
{
	DebugFunction();
	
	if ( !(vo_init&VO_INIT_IPU) )
	{
		printf("MAGX_VO: before get DMA mem need init IPU!\n");
		return 0;
	}
	
	pp_reqbufs_params pp_reqbufs;

	bool bMemAllowed;
	
	//Detect size IPU memory
	for ( int i=20;i>0;i-- )
	{
		pp_reqbufs.count = 1;
		pp_reqbufs.req_size = i*256*1024;
	
		//Try alloc memory
		bMemAllowed = (ioctl(fd_pp, PP_IOCTL_REQBUFS, &pp_reqbufs)==0);

		if ( bMemAllowed )
			break;
		
		// Free mem
		pp_reqbufs.count = 0;
		ioctl(fd_pp, PP_IOCTL_REQBUFS, &pp_reqbufs);		
	}
	
	//Get buffer
	pp_desc.index = 0;
	if (ioctl(fd_pp, PP_IOCTL_QUERYBUF, &pp_desc) != 0) 
	{
		perror("MAGX_VO: PP_IOCTL_QUERYBUF failed");
		return 0;
	}
	
	printf("MAGX_VO: IPU memory: %.1lf M\n", (double)pp_desc.size/(1024*1024));
	
	if ( pp_desc.size==0 )
	{
		pp_reqbufs.count = 0;
		ioctl(fd_pp, PP_IOCTL_REQBUFS, &pp_reqbufs);
		return 0;
	}
	
	//Initialize pool for dinamic alloc IPU memory
	ipu_pool_initialize(pp_desc.addr, pp_desc.size, IPU_PAGE_ALIGN);
	
	vo_init|=VO_ALLOC_DMA_MEM;
	return 1;
}

void uninitIPU()
{
	DebugFunction();
	
	if ( pp_dma_buffer )
	{
		int size = IPU_MEM_ALIGN(in_width*in_height*in_pixel_size);
		munmap(pp_dma_buffer,size);
		pp_dma_buffer=0;
		if ( vo_init&VO_INIT_DB )
		{
			ipu_free(pp_dma_buffer_addr);
			vo_init^=VO_INIT_DB;
		}
	}
	if ( vo_init&VO_ALLOC_IPU_BUF )
	{
		if ( pp_st.mid.addr )
			ipu_free(pp_st.mid.addr);
		if ( pp_st.in.addr && pp_st.in.index==-1 )
			ipu_free(pp_st.in.addr);
		memset(&pp_desc, 0, sizeof(pp_desc));
		vo_init^=VO_ALLOC_IPU_BUF;
	}
}

void uninit()
{
	DebugFunction();
	
	if (vo_init==0)
		return;
	
	if ( vo_init&VO_INIT_IPU )
	{		
		if (ioctl(fd_pp, PP_IOCTL_UNINIT, NULL) < 0)
			perror("MAGX_VO: PP_IOCTL_UNINIT");
		vo_init^=VO_INIT_IPU;
	}
	
	uninitIPU();
	
	if ( vo_init&VO_ALLOC_DMA_MEM )
	{
		pp_reqbufs_params pp_reqbufs;
		pp_reqbufs.count = 0;
		if (ioctl(fd_pp, PP_IOCTL_REQBUFS, &pp_reqbufs) < 0)
			perror("MAGX_VO: uninit PP_IOCTL_REQBUFS");
	}
	if ( fd_pp )
		close(fd_pp);
	if ( vo_init&VO_INIT_FB )
	{	
		if (ioctl(fb_dev_fd, FBIOPUT_VSCREENINFO, &fb_orig_vinfo))
			printf("MAGX_VO: Can't reset original fb_var_screeninfo: %s\n", strerror(errno));

		if ( pp_frame_buffer )
			memset(pp_frame_buffer, 0, p_width*p_height*hw_pixel_size);
		munmap(pp_frame_buffer, p_width*p_height*hw_pixel_size);
	}
	if (fb_dev_fd)
		close(fb_dev_fd);
	preinit();
	
	printf("MAGX_VO: All uninit\n");
}

#include "SDL_magx_cur.c"

void setCanUseCursor( bool use )
{
	bCanUseCursor = use;
}

void setMousPos( int x, int y )
{
	cursor_x = x;
	cursor_y = y;
}

void getMousPos( int &x, int &y )
{
	x = cursor_x;
	y = cursor_y;	
}

void setShowCursor( bool show )
{
	if ( bCanUseCursor )
		bShowCursor = show;
}

void drawMouse( char *dsc )
{
	if ( !bShowCursor )
		return;
	
	int w;

	w = in_width*2;
	dsc += cursor_x*2 + cursor_y*w;
	
	int cw=7, ch=12;
	if ( cw+cursor_x>=in_width ) cw=in_width-cursor_x;
	if ( ch+cursor_y>=in_height ) ch=in_height-cursor_y;
	if ( !cw || !ch ) return;
	
	char *src = (char *)sdlCursor;
	
	int i, j;
	for ( i=0; i<ch; i++ )
	{
		for ( j=0; j<cw*2; j+=2 )
		{
			if ( *((unsigned short int*)(src+j))!=0 )
				memcpy((dsc+j), (src+j), 2);
		}
		dsc += w;
		src += 16;
	}
}

void flipBuffer()
{
	DebugFunction();
	
	if ( pp_dma_buffer )
	{
		//line by line copy image
		char *src, *dst;
		int w, h;

		w = p_width*vo_pixel_size;
		h = p_height;
		src = pp_dma_buffer;
		dst = pp_frame_buffer;

		while ( h-- ) 
		{
			memcpy(dst, src, w);
			dst += w;
			src += w;
		}
	}
}

void flipPage()
{
	DebugFunction();
	
	drawMouse(pp_dma_buffer);
	
	if ( pp_st.in.index != -1 )
	{
		flipBuffer();
		return;
	}
	
	static int dma_start = 0;
	
	if (dma_start) 
	{
		if (ioctl(fd_pp, PP_IOCTL_WAIT, 0))
		{
			perror("MAGX_VO: PP_IOCTL_WAIT");
			return;
		} else
			dma_start = 0;
	}
	
	if (ioctl(fd_pp, PP_IOCTL_START, &pp_st) < 0) 
	{
		perror("MAGX_VO: PP_IOCTL_START");
		if (ioctl(fd_pp, PP_IOCTL_WAIT, 0))
			perror("MAGX_VO: PP_IOCTL_WAIT");
	} else
		dma_start = 1;
}

bool initDoubleBuffer()
{
	DebugFunction();
	
	printf("MAGX_VO: Create IPU buffer for DB\n");
	
	if ( pp_dma_buffer )
		return 1;
		
	unsigned int size = IPU_MEM_ALIGN(in_width*in_height*in_pixel_size);	
	pp_dma_buffer_addr = ipu_malloc( size );
	
	if ( pp_dma_buffer_addr<=0 )
	{
		printf("MAGX_VO: No memory for DB\n");
		pp_dma_buffer = NULL;		
		return 0;
	}
	
	pp_dma_buffer = (char*)mmap (NULL, size, 
						PROT_READ | PROT_WRITE, MAP_SHARED, 
							fd_pp, pp_dma_buffer_addr);
	if (pp_dma_buffer == MAP_FAILED) 
	{
		fprintf(stderr,"MAGX_VO: mmap IPU\n");
		pp_dma_buffer = NULL;
		return 0;
	}
	memset(pp_dma_buffer, 0, size);
	
	printf("MAGX_VO: alloced Double Buffer\n");
	
	vo_init|=VO_INIT_DB;
	return 1;
}

void setOriginalBPP(bool org)
{
	DebugFunction();
	
	if (org)
	{
		if ( vo_dbpp != hw_dbpp )
			setBppFB( hw_dbpp );
	} else
	{
		if ( vo_dbpp != in_dbpp )
			setBppFB( in_dbpp );	
	}
}

int reconfigureIPU( uint32_t width, uint32_t height, uint32_t in_bpp, uint16_t in_rot )
{
	DebugFunction();
	
	if ( width==in_width && height==in_height && in_bpp==in_dbpp )
		return 1;
		
	uninitIPU();
	
	return configureIPU( width, height, in_bpp, in_rot );
}

/// Info macro procedure

char * pixels()
{
	DebugFunction();
	
	if ( pp_dma_buffer )
		return pp_dma_buffer;
	else
		return pp_frame_buffer;
}

bool isRotate()
{
	return ((in_width < in_height)!=(p_width < p_height));
}

bool isScalling()
{
	return (( !isRotate() && (in_width>p_width || in_height>p_height) ) ||
			(  isRotate() && (in_width>p_height || in_height>p_width) )  );
}

bool isBppConvert()
{
	return (vo_dbpp!=in_dbpp);
}

////////////////////////////////////////////////////////////////////////

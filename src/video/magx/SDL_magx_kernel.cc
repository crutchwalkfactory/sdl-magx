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
static int in_height = 0;
static int in_width = 0;

//Out info
#define vo_dbpp (fb_vinfo.bits_per_pixel)
#define vo_pixel_size bppToPixelSize(vo_dbpp)

//IPU (PreProcessor)
static pp_start_params pp_st;
static char * pp_dma_buffer = NULL;
static pp_buf pp_desc;

//FB
static char * pp_frame_buffer = NULL;

//FB&IPU id
int fb_dev_fd;
int fd_pp;

//Init flag
int vo_init=0;
#define VO_INIT_FB			0x00000001
#define VO_INIT_IPU			0x00000002
#define VO_CONF_IPU			0x00000004
#define VO_ALLOC_DMA_MEM	0x00000008
#define VO_INIT_DB			0x00000010	

//DMA mem info
unsigned int iIPUMemSize;
unsigned int iIPUMemStart;
unsigned int iIPUMemFreeSize;
unsigned int iIPUMemFreeStart;

unsigned int iFBMemSize;

//FB info structure
struct fb_var_screeninfo fb_orig_vinfo;
struct fb_var_screeninfo fb_vinfo;
struct fb_fix_screeninfo fb_finfo;

//In info
int in_dbpp;

void signal_handler(int sig) 
{
	perror("MyExept\n");
	if ( vo_init )
		uninit(); 
}

void preinit()
{
	static int setSignal=1;
	if ( setSignal )
	{
		signal(SIGTERM, signal_handler);
		signal(SIGKILL, signal_handler);
		signal(SIGQUIT, signal_handler);
		setSignal = 0;
	}
	fb_dev_fd=fd_pp=0;	
	iIPUMemFreeSize=iIPUMemSize=0;
	iIPUMemFreeStart=iIPUMemStart=0;
	pp_dma_buffer=0;
}

//Open and preinicialization FB
int initFB()
{
	if ((fb_dev_fd = open("/dev/fb/0", O_RDWR)) == -1) 
	{
		printf("MAGX_VO: Can't open /dev/fb/0: %s\n", strerror(errno));
		goto err_out;
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

	iFBMemSize = fb_finfo.smem_len;
	
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
	
err_out:
	return 0;	
}

int initIPU()
{
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

int configureIPU( uint32_t width, uint32_t height, uint32_t in_bpp, uint16_t in_rot )
{
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
	
	pp_init_params pp_init;
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
	switch ( in_dbpp )
	{		
		case 16:
			pp_init.in_pixel_fmt = IPU_PIX_FMT_RGB565;
			break;
		case 32:
			pp_init.in_pixel_fmt = IPU_PIX_FMT_BGR32;
			break;		
		case 24:
		default:
			pp_init.in_pixel_fmt = IPU_PIX_FMT_RGB24;
			break;
	}
	
	pp_init.out_width  = p_width;
	pp_init.out_height = p_height;
	pp_init.out_stride = p_width;
	pp_init.mid_stride = p_height;
	pp_init.rot = rot;
	switch ( vo_dbpp )
	{
		case 16:
			pp_init.out_pixel_fmt = IPU_PIX_FMT_RGB565;
			break;	
		case 32:
			pp_init.out_pixel_fmt = IPU_PIX_FMT_BGR32;
			break;				
		case 24:
		default:
			pp_init.out_pixel_fmt = IPU_PIX_FMT_BGRA6666;
			break;			
	}

	if (ioctl(fd_pp, PP_IOCTL_INIT, &pp_init) < 0) 
	{
		perror("MAGX_VO: PP_IOCTL_INIT");
		return 0;
	}
	
	memset(&pp_st, 0, sizeof(pp_st));
	pp_st.wait = 0;
	if ( pp_dma_count>1 )
	{
		pp_st.mid = pp_desc;
		pp_st.mid.size = IPU_MEM_ALIGN(p_width*p_height*vo_pixel_size);
		iIPUMemFreeSize-=pp_st.mid.size;
		iIPUMemFreeStart+=pp_st.mid.size;
	}
	if ( pp_dma_count>0 )
	{
		pp_st.in.index = -1;
		pp_st.in.addr = iIPUMemFreeStart;
		pp_st.in.size = IPU_MEM_ALIGN(vwidth*vheight*in_pixel_size);
		iIPUMemFreeSize-=pp_st.in.size;
		iIPUMemFreeStart+=pp_st.in.size;
		
		pp_dma_buffer = (char*)mmap (NULL, pp_st.in.size, 
						PROT_READ | PROT_WRITE, MAP_SHARED, 
							fd_pp, pp_st.in.addr);
		
		pp_st.out.index = -1;
		pp_st.out.size = IPU_MEM_ALIGN(p_width*p_height*vo_pixel_size);
		pp_st.out.addr = fb_finfo.smem_start;
	}
	
	return 1;
}

int getAllDMAMem()
{
	if ( !(vo_init&VO_INIT_IPU) )
	{
		printf("MAGX_VO: before get DMA mem need init IPU!\n");
		return 0;
	}
	
	pp_reqbufs_params pp_reqbufs;

	bool bMemAllowed=false;

	for ( int i=10;i>0&&!bMemAllowed;i-- )
	{
		pp_reqbufs.count = 1;
		pp_reqbufs.req_size = i*512*1024;
	
		//Try alloc memory
		bMemAllowed = (ioctl(fd_pp, PP_IOCTL_REQBUFS, &pp_reqbufs)==0);

		if ( bMemAllowed )
		{
			iIPUMemFreeSize=iIPUMemSize=pp_reqbufs.req_size;
			break;
		}
		
		// Free mem
		pp_reqbufs.count = 0;
		ioctl(fd_pp, PP_IOCTL_REQBUFS, &pp_reqbufs);		
	}
	printf("MAGX_VO: IPU memory: %uM\n", iIPUMemSize/(1024*1024));
	
	if ( iIPUMemSize<=0 )
		return 0;
	
	pp_desc.index = 0;
	if (ioctl(fd_pp, PP_IOCTL_QUERYBUF, &pp_desc) != 0) 
	{
		perror("MAGX_VO: PP_IOCTL_QUERYBUF failed");
		return 0;
	}
	iIPUMemFreeStart=iIPUMemStart=pp_desc.addr;
	
	vo_init|=VO_ALLOC_DMA_MEM;
	return 1;
}

void uninit()
{
	if ( pp_dma_buffer )
	{
		munmap(pp_dma_buffer, IPU_MEM_ALIGN(in_width*in_height*in_pixel_size));
		pp_dma_buffer=0;
	}
	if ( vo_init&VO_ALLOC_DMA_MEM )
	{
		pp_reqbufs_params pp_reqbufs;
		pp_reqbufs.count = 0;
		if (ioctl(fd_pp, PP_IOCTL_REQBUFS, &pp_reqbufs) < 0)
			perror("MAGX_VO: uninit PP_IOCTL_REQBUFS");
	}
	if ( vo_init&VO_INIT_IPU )
	{		
		if (ioctl(fd_pp, PP_IOCTL_UNINIT, NULL) < 0)
			perror("MAGX_VO: PP_IOCTL_UNINIT");
		close(fd_pp);
	}
	if ( vo_init&VO_INIT_FB )
	{	
		if (ioctl(fb_dev_fd, FBIOPUT_VSCREENINFO, &fb_orig_vinfo))
			printf("MAGX_VO: Can't reset original fb_var_screeninfo: %s\n", strerror(errno));

		if ( pp_frame_buffer )
			memset(pp_frame_buffer, 0, p_width*p_height*hw_pixel_size);
		munmap(pp_frame_buffer, p_width*p_height*hw_pixel_size);

		close(fb_dev_fd);
	}
	preinit();
	
	printf("MAGX_VO: All uninit\n");
}

void flipBuffer()
{
	char *src, *dst;
	int w, h;
	int srcskip, dstskip;

	w = p_width*vo_pixel_size;
	h = p_height;
	src = pp_dma_buffer;
	dst = pp_frame_buffer;

	while ( h-- ) 
		memcpy(dst, src, p_width);
}

void flipPage()
{
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
	printf("MAGX_VO: Create IPU buffer for DB\n");
	
	if ( pp_dma_buffer )
		return 1;
		
	int size_buffer = IPU_MEM_ALIGN(in_width*in_height*in_pixel_size);	
	
	pp_dma_buffer = (char*)mmap (NULL, size_buffer, 
						PROT_READ | PROT_WRITE, MAP_SHARED, 
							fd_pp, iIPUMemFreeStart);
	if (pp_dma_buffer == MAP_FAILED) 
	{
		fprintf(stderr,"MAGX_VO: mmap IPU 0\n");
		return 0;
	}
	memset(pp_dma_buffer, 0, size_buffer);

	iIPUMemFreeStart+=size_buffer;
	iIPUMemFreeSize-=size_buffer;
	
	printf("MAGX_VO: alloced IPU buffer\n");
	
	vo_init|=VO_INIT_DB;
	return 1;
}

void setOriginalBPP(bool org)
{
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

/// Info macro procedure

char * pixels()
{
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


////////////////////////////////////////////////////////////////////////

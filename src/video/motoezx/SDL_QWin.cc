/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2006 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

#include "SDL_QWin.h"
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <ZApplication.h>
#include <ZKeyDef.h>
#include <qcopchannel_qws.h>
#include <qtimer.h>

/* Name of the environment variable used to invert the screen rotation or not:
Possible values:
!=0 : Screen is 270° rotated
0: Screen is 90° rotated*/
#define SDL_QT_ROTATION_ENV_NAME "SDL_QT_INVERT_ROTATION"

screenRotationT screenRotation = SDL_QT_NO_ROTATION;

#ifdef __cplusplus
extern "C" {
#endif
//For compobility
void SDL_ChannelExists(const char *){};
void SDL_ShowSplash(){};
void SDL_HideSplash(){};
//For suspend on call
extern int UTIL_GetIncomingCallStatus();
extern int UTIL_GetSideKeyLock();
//For load keymap
extern void CargarTeclas();
#ifdef __cplusplus
}
#endif

static inline bool needSuspend()
{
  if(UTIL_GetIncomingCallStatus() || UTIL_GetSideKeyLock())
    return true;
  return false;
}

int myRED, myCENTER, myUP, myDOWN, myLEFT, myRIGHT, mySIDE, myMUSIC, myC, myLSOFT, myRSOFT, myCALL, myCAMERA, myVOLUP, myVOLDOWN, my0, my1, my2, my3, my4, my5, my6, my7, my8, my9, myASTERISK, myNUMERAL;
int SmyRED, SmyCENTER, SmyUP, SmyDOWN, SmyLEFT, SmyRIGHT, SmySIDE, SmyMUSIC, SmyC, SmyLSOFT, SmyRSOFT, SmyCALL, SmyCAMERA, SmyVOLUP, SmyVOLDOWN, Smy0, Smy1, Smy2, Smy3, Smy4, Smy5, Smy6, Smy7, Smy8, Smy9, SmyASTERISK, SmyNUMERAL;

void CargarTeclas()
{
	ZConfig mySDL(QString(qApp->argv()[0])+"_SDL.cfg", false);
	
	//Get default rotation
	int envValue;
	char * envString = SDL_getenv(SDL_QT_ROTATION_ENV_NAME);
	if ( envString )
		envValue = atoi(envString);
	else
		envValue = mySDL.readNumEntry("SYSTEM", "Rotation", 1);		  
	screenRotation = envValue ? SDL_QT_ROTATION_90 : SDL_QT_ROTATION_270;
	
	
	ZConfig keyCFG("/usr/mlib/SDL/keyconfig.cfg", false);
	QString val;
	val = mySDL.readEntry("SDL", "Red", "ESCAPE");
	myRED = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_ESCAPE);
	val = mySDL.readEntry("SDL", "Center", "RETURN");
	myCENTER = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_RETURN);
	val = mySDL.readEntry("SDL", "Up", "UP");
	myUP = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_UP);
	val = mySDL.readEntry("SDL", "Down", "DOWN");
	myDOWN = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_DOWN);
	val = mySDL.readEntry("SDL", "Left", "LEFT");
	myLEFT = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_LEFT);
	val = mySDL.readEntry("SDL", "Right", "RIGHT");
	myRIGHT = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_RIGHT);
	val = mySDL.readEntry("SDL", "SideKey", "TAB");
	mySIDE = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_TAB);
	val = mySDL.readEntry("SDL", "Music", "F10");
	myMUSIC = keyCFG.readNumEntry("KEYCODES", QString(val), 291);
	val = mySDL.readEntry("SDL", "C", "BACKSPACE");
	myC = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_BACKSPACE);
	val = mySDL.readEntry("SDL", "LeftSoftkey", "F9");
	myLSOFT = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_F9);
	val = mySDL.readEntry("SDL", "RightSoftkey", "F11");
	myRSOFT = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_F11);
	val = mySDL.readEntry("SDL", "Call", "SPACE");
	myCALL = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_SPACE);
	val = mySDL.readEntry("SDL", "Camera", "PAUSE");
	myCAMERA = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_PAUSE);
	val = mySDL.readEntry("SDL", "VolumeUp", "PLUS");
	myVOLUP = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_PLUS);
	val = mySDL.readEntry("SDL", "VolumeDown", "MINUS");
	myVOLDOWN = keyCFG.readNumEntry("KEYCODES", QString(val), SDLK_MINUS);
	val = mySDL.readEntry("SDL", "0", "0");
	my0 = keyCFG.readNumEntry("KEYCODES", QString(val), 48);
	val = mySDL.readEntry("SDL", "1", "1");
	my1 = keyCFG.readNumEntry("KEYCODES", QString(val), 49);
	val = mySDL.readEntry("SDL", "2", "2");
	my2 = keyCFG.readNumEntry("KEYCODES", QString(val), 50);
	val = mySDL.readEntry("SDL", "3", "3");
	my3 = keyCFG.readNumEntry("KEYCODES", QString(val), 51);
	val = mySDL.readEntry("SDL", "4", "4");
	my4 = keyCFG.readNumEntry("KEYCODES", QString(val), 52);
	val = mySDL.readEntry("SDL", "5", "5");
	my5 = keyCFG.readNumEntry("KEYCODES", QString(val), 53);
	val = mySDL.readEntry("SDL", "6", "6");
	my6 = keyCFG.readNumEntry("KEYCODES", QString(val), 54);
	val = mySDL.readEntry("SDL", "7", "7");
	my7 = keyCFG.readNumEntry("KEYCODES", QString(val), 55);
	val = mySDL.readEntry("SDL", "8", "8");
	my8 = keyCFG.readNumEntry("KEYCODES", QString(val), 56);
	val = mySDL.readEntry("SDL", "9", "9");
	my9 = keyCFG.readNumEntry("KEYCODES", QString(val), 57);
	val = mySDL.readEntry("SDL", "Asterisk", "ASTERISK");
	myASTERISK = keyCFG.readNumEntry("KEYCODES", QString(val), 42);
	val = mySDL.readEntry("SDL", "Numeral", "HASH");
	myNUMERAL = keyCFG.readNumEntry("KEYCODES", QString(val), 35);
	
	val = mySDL.readEntry("SDLextra", "Red", "");
	SmyRED = keyCFG.readNumEntry("KEYCODES", QString(val), myRED);
	val = mySDL.readEntry("SDLextra", "Center", "");
	SmyCENTER = keyCFG.readNumEntry("KEYCODES", QString(val), myCENTER);
	val = mySDL.readEntry("SDLextra", "Up", "");
	SmyUP = keyCFG.readNumEntry("KEYCODES", QString(val), myUP);
	val = mySDL.readEntry("SDLextra", "Down", "");
	SmyDOWN = keyCFG.readNumEntry("KEYCODES", QString(val), myDOWN);
	val = mySDL.readEntry("SDLextra", "Left", "");
	SmyLEFT = keyCFG.readNumEntry("KEYCODES", QString(val), myLEFT);
	val = mySDL.readEntry("SDLextra", "Right", "");
	SmyRIGHT = keyCFG.readNumEntry("KEYCODES", QString(val), myRIGHT);
	val = mySDL.readEntry("SDLextra", "SideKey", "");
	SmySIDE = keyCFG.readNumEntry("KEYCODES", QString(val), mySIDE);
	val = mySDL.readEntry("SDLextra", "Music", "");
	SmyMUSIC = keyCFG.readNumEntry("KEYCODES", QString(val), myMUSIC);
	val = mySDL.readEntry("SDLextra", "C", "");
	SmyC = keyCFG.readNumEntry("KEYCODES", QString(val), myC);
	val = mySDL.readEntry("SDLextra", "LeftSoftkey", "");
	SmyLSOFT = keyCFG.readNumEntry("KEYCODES", QString(val), myLSOFT);
	val = mySDL.readEntry("SDLextra", "RightSoftkey", "");
	SmyRSOFT = keyCFG.readNumEntry("KEYCODES", QString(val), myRSOFT);
	val = mySDL.readEntry("SDLextra", "Call", "");
	SmyCALL = keyCFG.readNumEntry("KEYCODES", QString(val), myCALL);
	val = mySDL.readEntry("SDLextra", "Camera", "");
	SmyCAMERA = keyCFG.readNumEntry("KEYCODES", QString(val), myCAMERA);
	val = mySDL.readEntry("SDLextra", "VolumeUp", "");
	SmyVOLUP = keyCFG.readNumEntry("KEYCODES", QString(val), myVOLUP);
	val = mySDL.readEntry("SDLextra", "VolumeDown", "");
	SmyVOLDOWN = keyCFG.readNumEntry("KEYCODES", QString(val), myVOLDOWN);
	val = mySDL.readEntry("SDLextra", "0", "");
	Smy0 = keyCFG.readNumEntry("KEYCODES", QString(val), my0);
	val = mySDL.readEntry("SDLextra", "1", "");
	Smy1 = keyCFG.readNumEntry("KEYCODES", QString(val), my1);
	val = mySDL.readEntry("SDLextra", "2", "");
	Smy2 = keyCFG.readNumEntry("KEYCODES", QString(val), my2);
	val = mySDL.readEntry("SDLextra", "3", "");
	Smy3 = keyCFG.readNumEntry("KEYCODES", QString(val), my3);
	val = mySDL.readEntry("SDLextra", "4", "");
	Smy4 = keyCFG.readNumEntry("KEYCODES", QString(val), my4);
	val = mySDL.readEntry("SDLextra", "5", "");
	Smy5 = keyCFG.readNumEntry("KEYCODES", QString(val), my5);
	val = mySDL.readEntry("SDLextra", "6", "");
	Smy6 = keyCFG.readNumEntry("KEYCODES", QString(val), my6);
	val = mySDL.readEntry("SDLextra", "7", "");
	Smy7 = keyCFG.readNumEntry("KEYCODES", QString(val), my7);
	val = mySDL.readEntry("SDLextra", "8", "");
	Smy8 = keyCFG.readNumEntry("KEYCODES", QString(val), my8);
	val = mySDL.readEntry("SDLextra", "9", "");
	Smy9 = keyCFG.readNumEntry("KEYCODES", QString(val), my9);
	val = mySDL.readEntry("SDLextra", "Asterisk", "");
	SmyASTERISK = keyCFG.readNumEntry("KEYCODES", QString(val), myASTERISK);
	val = mySDL.readEntry("SDLextra", "Numeral", "");
	SmyNUMERAL = keyCFG.readNumEntry("KEYCODES", QString(val), myNUMERAL);
}

////////////////////////////////////////////////////////////////////////
//////////////////////////  IPU + FB  //////////////////////////////////
////////////////////////////////////////////////////////////////////////
//
// Motorola IPU and FB init and configuration for SDL
//		by Ant-ON <prozanton@gmail.com>
//		(C) 2011
//
// Based on:
//		video driver whith IPU for MPlayer 
//		by Alexey Kuznetsov <kuznet@ms2.inr.ac.ru>   
//		(C) 2008
//



#include "ipu.h"
#include "mxcfb.h"
#include "mxc_pp.h"

static int vo_inited=0;

//FB
static int fb_dev_fd;// vo_fbdev related variables

static struct fb_var_screeninfo fb_orig_vinfo;
static struct fb_var_screeninfo fb_vinfo;
static struct fb_fix_screeninfo fb_finfo;

#define hw_dbpp (fb_orig_vinfo.bits_per_pixel)
#define vo_dbpp (fb_vinfo.bits_per_pixel)
static int in_dbpp = 0;

#define bppToPixelSize(a) ((((a)-1)/8)+1)

#define hw_pixel_size bppToPixelSize(hw_dbpp)
#define vo_pixel_size bppToPixelSize(vo_dbpp)
#define in_pixel_size bppToPixelSize(in_dbpp)

static int dma_start = 0;

//IPU
#define p_height (fb_orig_vinfo.yres)
#define p_width (fb_orig_vinfo.xres)
static int in_height = 0;
static int in_width = 0;

static int fd_pp;
static int pp_dma_count=0;
static char * pp_dma_buffer[2];
static pp_buf pp_desc[2];

static pp_buf pp_frame;
static char * pp_frame_buffer = NULL;

static pp_start_params pp_st;

extern "C" 
{
unsigned int iIPUMemSize;
unsigned int iIPUMemStart;
unsigned int iIPUMemFreeSize;
unsigned int iIPUMemFreeStart;

unsigned int iFBMemSize;
};

static int initFB()
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

	pp_frame.index = -1;
	pp_frame.size = fb_finfo.smem_len;
	pp_frame.addr = fb_finfo.smem_start;
	iFBMemSize = fb_finfo.smem_len;
	
	if ((pp_frame_buffer = (char*)mmap(0, p_width*p_height*hw_pixel_size, PROT_READ | PROT_WRITE,
				 MAP_SHARED, fb_dev_fd, 0)) == MAP_FAILED) 
	{
		printf( "MAGX_VO: Can't mmap %s\n", strerror(errno));
		return 0;
	}
	
	memset(pp_frame_buffer, 0, p_width*p_height*hw_pixel_size);
	
	return 1;
	
err_out_fd:
	close(fb_dev_fd);
	fb_dev_fd = -1;
	
err_out:
	return 0;	
}

extern "C" bool isRotate()
{
	return ((in_width < in_height)!=(p_width < p_height));
}

extern "C" bool isScalling()
{
	return (( !isRotate() && (in_width>p_width || in_height>p_height) ) ||
			(  isRotate() && (in_width>p_height || in_height>p_width) )  );
}

static int setBppFB( uint32_t in_bpp )
{
	if (!fb_dev_fd)
		return 0;
	
	printf("MAGX_VO: set new bpp %d (%d) on FB\n", in_bpp, fb_vinfo.bits_per_pixel);

	if ( in_bpp != fb_vinfo.bits_per_pixel )
	{
		printf("MAGX_VO: switching to bpp: %d\n", in_bpp);
		fb_vinfo.bits_per_pixel = in_bpp;
		
		if (ioctl(fb_dev_fd, FBIOPUT_VSCREENINFO, &fb_vinfo)) 
		{
			printf("MAGX_VO: Can't set VSCREENINFO: %s\n", strerror(errno));
			goto err_out_fd;
		}
		if (ioctl(fb_dev_fd, FBIOGET_VSCREENINFO, &fb_vinfo)) 
		{
			printf("MAGX_VO: Can't get VSCREENINFO: %s\n", strerror(errno));
			goto err_out_fd;
		}
		if ( in_bpp != fb_vinfo.bits_per_pixel )
		{
			printf("MAGX_VO: ERROR switching to bpp: %d\n", in_bpp);
			return 0;
		}
	}
	
	printf("MAGX_VO: seted bpp on FB\n");

	return 1;

err_out_fd:
	close(fb_dev_fd);
	fb_dev_fd = -1;
	return 0;
}

static void setScrBppFB(bool bOn)
{
	if (bOn)
	{
		if ( vo_dbpp != hw_dbpp )
			setBppFB( hw_dbpp );
	} else
	{
		if ( vo_dbpp != in_dbpp )
			setBppFB( in_dbpp );		
	}
}

static int initIPU( uint32_t width, uint32_t height, uint32_t in_bpp )
{
	pp_frame.size = p_width*p_height*vo_pixel_size;
	
	in_dbpp = in_bpp;
	in_height = height;
	in_width = width;
	
	bool dontrot = 0;
	pp_dma_count=0;	
	
	if ( (width < height)==(p_width < p_height) ) 
	{
		dontrot = 1;
		screenRotation = SDL_QT_NO_ROTATION;
	}
	else
	{
		pp_dma_count++;
		printf("MAGX_VO: screen rotated\n");
	}
		
	/* width must be aligned to 8, IPU does not accept unaligned
	 * values. This is not required for height, however I observed
	 * some strange solor artfacts when it is not aligned.
	 */
	int vwidth, vheight;
	vwidth = (width + 7) & ~7;
	vheight = (height + 7) & ~7;
	
	printf("MAGX_VO: vwidth=%d vheight=%d hw_pixel_size=%d\n",
	       vwidth, vheight, vo_pixel_size);

	int i;
	pp_init_params pp_init;
	pp_reqbufs_params pp_reqbufs;

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
	
	if ( (vo_dbpp!=in_dbpp) || 
			( dontrot && (width>p_width || height>p_height) ) ||
			(!dontrot && (width>p_height || height>p_width) )     
					)
	{
		pp_dma_count++;
		printf("MAGX_VO: screen scalling or bpp convert\n");
	}
	
	memset(&pp_init, 0, sizeof(pp_init));

	if ( pp_dma_count>1 )
		pp_init.mode = PP_PP_ROT;
	else
	if ( pp_dma_count==1 )
	{
		if ( !dontrot )
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
	pp_init.rot = dontrot ? IPU_ROTATE_NONE : 
		(screenRotation==SDL_QT_ROTATION_270?IPU_ROTATE_90_LEFT:IPU_ROTATE_90_RIGHT);
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

	//Get size of IPU memory
	pp_reqbufs.count = 1;

	bool bMemAllowed=true;
	iIPUMemSize=0;

	for ( int i=5;i>0;i-- )
	{
		pp_reqbufs.req_size = i*1024*1024;
	
		//Try alloc memory
		bMemAllowed = (ioctl(fd_pp, PP_IOCTL_REQBUFS, &pp_reqbufs)==0);

		if ( bMemAllowed )
		{
			iIPUMemFreeSize=iIPUMemSize=pp_reqbufs.req_size;
			break;
		}
	}
	printf("MAGX_VO: IPU memory: %uM\n", iIPUMemSize/(1024*1024));
	
	// Free mem
	pp_reqbufs.count = 0;
	ioctl(fd_pp, PP_IOCTL_REQBUFS, &pp_reqbufs);
	
	//Get buffer
	pp_reqbufs.count = pp_dma_count?(pp_dma_count>1?2:PP_MAX_BUFFER_CNT):0;
	pp_reqbufs.req_size = p_width*p_height*vo_pixel_size;
	if (pp_reqbufs.req_size < (vwidth*vheight*in_pixel_size))
			pp_reqbufs.req_size = (vwidth*vheight*in_pixel_size);

	if ( ioctl(fd_pp, PP_IOCTL_REQBUFS, &pp_reqbufs)!=0 )
	{
		perror("MAGX_VO: PP_IOCTL_REQBUFS");
		return 0;
	}
	
	for (i = 0; i < pp_dma_count; i++)
	{
		pp_desc[i].index = i;
		if (ioctl(fd_pp, PP_IOCTL_QUERYBUF, &pp_desc[i]) != 0) 
		{
			perror("MAGX_VO: PP_IOCTL_QUERYBUF failed");
			return 0;
		}
	}
	
	iIPUMemFreeStart=iIPUMemStart=pp_desc[0].addr;
	
	if (pp_dma_count>1)
		pp_desc[1].size = IPU_MEM_ALIGN(p_width*p_height*vo_pixel_size);
	
	for (i = 0; i < pp_dma_count; i++)
	{
		pp_dma_buffer[i] = (char*)mmap (NULL, pp_desc[i].size, 
							PROT_READ | PROT_WRITE, MAP_SHARED, 
								fd_pp, pp_desc[i].addr);
		if (pp_dma_buffer[i] == MAP_FAILED) 
		{
			fprintf(stderr,"MAGX_VO: mmap IPU %d\n", i);
			return 0;
		}
		
		memset(pp_dma_buffer[i], 0, pp_desc[i].size);
	
		iIPUMemFreeStart+=pp_desc[i].size;
		iIPUMemFreeSize-=pp_desc[i].size;
	}

	memset(&pp_st, 0, sizeof(pp_st));
	pp_st.wait = 0;
	pp_st.in = pp_desc[0];
	if ( pp_dma_count>1 )
		pp_st.mid = pp_desc[1];
	pp_st.out = pp_frame;	

	printf("MAGX_VO: IPU inited!\n");
	
	vo_inited=1;
	
	return 1;
}

static void uninit()
{
	if (dma_start) 
	{
		if (ioctl(fd_pp, PP_IOCTL_WAIT, 0))
			perror("MAGX_VO: PP_IOCTL_WAIT");
		else
			dma_start = 0;
	}

	pp_reqbufs_params pp_reqbufs;
	pp_reqbufs.count = 0;
	if (ioctl(fd_pp, PP_IOCTL_REQBUFS, &pp_reqbufs) < 0)
		perror("MAGX_VO: uninit PP_IOCTL_REQBUFS");

	if (ioctl(fd_pp, PP_IOCTL_UNINIT, NULL) < 0)
		perror("MAGX_VO: PP_IOCTL_UNINIT");

	for (int i=0;i<pp_dma_count;i++)
		munmap(pp_dma_buffer[i], pp_desc[i].size);

	close(fd_pp);

	if (ioctl(fb_dev_fd, FBIOPUT_VSCREENINFO, &fb_orig_vinfo))
		printf("MAGX_VO: Can't reset original fb_var_screeninfo: %s\n", strerror(errno));

	if ( pp_frame_buffer )
		memset(pp_frame_buffer, 0, p_width*p_height*hw_pixel_size);
	munmap(pp_frame_buffer, pp_frame.size);

	close(fb_dev_fd);
	
	pp_frame.addr = NULL;
	pp_frame_buffer = NULL;
	
	pp_dma_count=0;
	
	vo_inited=0;
	
	perror("MAGX_VO: All uninit");
}

static void flipPage()
{
	if (dma_start) 
	{
		if (ioctl(fd_pp, PP_IOCTL_WAIT, 0))
			perror("MAGX_VO: PP_IOCTL_WAIT");
		else
			dma_start = 0;
	}
	if (dma_start)
		return;

	//printf("MAGX_VO: flip\n"); fflush(stdout);

	if (ioctl(fd_pp, PP_IOCTL_START, &pp_st) < 0) 
	{
		perror("MAGX_VO: PP_IOCTL_START");
		if (ioctl(fd_pp, PP_IOCTL_WAIT, 0))
			perror("MAGX_VO: PP_IOCTL_WAIT");
	} else
		dma_start = 1;
}

extern "C" bool reinitWithBuffer()
{
	printf("MAGX_VO: Create IPU buffer for DB\n");
	
	if ( pp_dma_count!=0 ) return 0;
	pp_dma_count=1;

	pp_reqbufs_params pp_reqbufs;
	pp_reqbufs.count = pp_dma_count;
	pp_reqbufs.req_size = p_width*p_height*vo_pixel_size;

	if ( ioctl(fd_pp, PP_IOCTL_REQBUFS, &pp_reqbufs)!=0 )
	{
		perror("MAGX_VO: PP_IOCTL_REQBUFS");
		return 0;
	}
	
	pp_desc[0].index = 0;
	if ( ioctl(fd_pp, PP_IOCTL_QUERYBUF, &pp_desc[0]) != 0 ) 
	{
		perror("MAGX_VO: PP_IOCTL_QUERYBUF failed");
		return 0;
	}

	pp_dma_buffer[0] = (char*)mmap (NULL, pp_desc[0].size, 
						PROT_READ | PROT_WRITE, MAP_SHARED, 
							fd_pp, pp_desc[0].addr);
	if (pp_dma_buffer[0] == MAP_FAILED) 
	{
		fprintf(stderr,"MAGX_VO: mmap IPU 0\n");
		return 0;
	}
	
	memset(pp_dma_buffer[0], 0, pp_desc[0].size);

	iIPUMemFreeStart+=pp_desc[0].size;
	iIPUMemFreeSize-=pp_desc[0].size;
	
	printf("MAGX_VO: alloced IPU buffer\n");
	
	return 1;
}

static void flipBuffer()
{
	char *src, *dst;
	int w, h;
	int srcskip, dstskip;

	w = p_width*vo_pixel_size;
	h = p_height;
	src = pp_dma_buffer[0];
	dst = pp_frame_buffer;

	while ( h-- ) 
		SDL_memcpy(dst, src, p_width);
}

////////////////////////////////////////////////////////////////////////

#include <signal.h>

void signal_handler(int sig) 
{
	perror("MyExept\n");
	if ( vo_inited )
		uninit(); 
}

SDL_ZWin::SDL_ZWin(const QSize& size)
    :ZKbMainWidget ( ZHeader::MAINDISPLAY_HEADER, 0, "SDL", 0),
    my_mouse_pos(0, 0), my_special(false),
    my_suspended(false), last_mod(false)
{
	signal(SIGTERM, signal_handler);
	signal(SIGKILL, signal_handler);
	signal(SIGQUIT, signal_handler);	

	CargarTeclas();
	
	last.scancode = 0;
  	
  	disconnect( qApp, SIGNAL(askReturnToIdle(int)), qApp, SLOT(slotReturnToIdle(int)) );
  	
	connect(qApp, SIGNAL(signalRaise()), this, SLOT(slotRaise()));
	connect(qApp, SIGNAL(askReturnToIdle(int)), this, SLOT(slotReturnToIdle(int))); 
	  	
    inCallChannel = new QCopChannel("/EZX/PHONE/INCOMING_CALL", this);
    connect( inCallChannel, SIGNAL(received(const QCString&, const QByteArray&)), this, SLOT(channel(const QCString&, const QByteArray&)));
    ounCallChannel = new QCopChannel("/EZX/PHONE/OUTGOING_CALL", this);
    connect( ounCallChannel, SIGNAL(received(const QCString&, const QByteArray&)), this, SLOT(channel(const QCString&, const QByteArray&)));
	
	setFocusPolicy(QWidget::StrongFocus);
	setFullScreenMode(true);
	qApp->installEventFilter( this );
	
	isOk = initFB();
}

SDL_ZWin::~SDL_ZWin() 
{
	delete inCallChannel;
	delete ounCallChannel;
	printf("MAGX: ~SDL_QWin(): done\n");
}

bool SDL_ZWin::SetVideoMode(uint32_t width, uint32_t height, uint32_t in_bpp)
{
	if ( in_bpp >= 24 )
		printf("MAGX: Warning! Not recommended use bpp=%d, it very slow!\n", in_bpp);
	
	if ( vo_inited  )
	{
		if (width==in_width && height==in_height && in_bpp==in_dbpp)
		{
			memset(pp_frame_buffer, 0, iFBMemSize);
			return 1;
		} else 
			uninit();
	}
	
	setBppFB( in_bpp );
	
	//Show logo
	FILE * f = fopen("/mmc/mmca1/sdllogo.bin", "rb");
	if ( !f ) f = fopen("/usr/mlib/SDL/sdllogo.bin", "rb");	
	if ( !f ) f = fopen("/mmc/mmca1/games/lib/sdllogo.bin", "rb");
	if ( !f ) f = fopen("/ezxlocal/download/mystuff/games/lib/sdllogo.bin", "rb");
	if ( f )
	{
		fread(pp_frame_buffer, p_width*p_height*2, 1, f);
		fclose(f);
		sleep(1);
	}

	return initIPU( width, height, in_bpp );
}

void * SDL_ZWin::getFBBuf()
{
	if ( pp_dma_count )
		return (void *)pp_dma_buffer[0];
	else
		return (void *)pp_frame_buffer;
}

void SDL_ZWin::flipScreen()
{
	if ( FocusOut || my_suspended )
		return;
	
	//Fix: skipping the first update screen, for normal show logo SDL
	if ( vo_inited==1 ) 
	{
		vo_inited++;
		//printf("MAGX: skip first flip\n");
		return;
	}
	
	flipPage();
}

void SDL_ZWin::flipScreen2()
{
	if ( FocusOut || my_suspended )
		return;
	
	//Fix: skipping the first update screen, for normal show logo SDL
	if ( vo_inited==1 ) 
	{
		vo_inited++;
		//printf("MAGX: skip first flip\n");
		return;
	}
	
	flipBuffer();
}

void SDL_ZWin::uninitVideo()
{
	uninit();
}

void SDL_ZWin::channel(const QCString &msg, const QByteArray &)
{
 	if ( msg == "show()" ) 
	{	
		resume();
	} else 
	if ( msg == "hide()" || msg == "MO" )
	{
		suspend();
	} else
	if (  msg == "ON" )
	{
		suspend(2);
	} else
	if (  msg == "OFF" && my_suspended==2 )
	{	
		resume();
	}
}

void SDL_ZWin::slotRaise()
{
	printf("MAGX: signal raise\n");
	resume();
}

void SDL_ZWin::slotReturnToIdle(int)
{
	printf("MAGX: signal askReturnToIdle\n");
	suspend();
}

void SDL_ZWin::suspend( int n )
{
	printf("MAGX: suspend\n");
	if (my_suspended)
	{
		hide();  
		return;
	}
	my_suspended = n;
	hide();
}

void SDL_ZWin::focusInEvent( QFocusEvent * )
{
	printf("MAGX: focus in\n");	
	if ( vo_inited )
		setScrBppFB(0);
	FocusOut=0;
	SDL_PrivateAppActive(true, SDL_APPINPUTFOCUS);
}

void SDL_ZWin::focusOutEvent( QFocusEvent * )
{
	printf("MAGX: focus out\n");
	FocusOut=1;
	if ( vo_inited )
		setScrBppFB(1);	
	SDL_PrivateAppActive(false, SDL_APPINPUTFOCUS);
}

void SDL_ZWin::resume()
{
	if(!my_suspended || needSuspend()) return;
	printf("MAGX: resume\n");
	show();
	
	my_suspended = 0;
}

void SDL_ZWin::closeEvent(QCloseEvent *e) 
{
	SDL_PrivateQuit();
	e->ignore();
}

void SDL_ZWin::setMousePos(const QPoint &pos) 
{
	if (screenRotation == SDL_QT_NO_ROTATION)
		my_mouse_pos = pos;		
	else if (screenRotation == SDL_QT_ROTATION_270)
		my_mouse_pos = QPoint(height()-pos.y(), pos.x());
	else if (screenRotation == SDL_QT_ROTATION_90)
		my_mouse_pos = QPoint(pos.y(), width()-pos.x());
}

inline int SDL_ZWin::keyUp()
{
  return my_special ? SmyUP : myUP;
}

inline int SDL_ZWin::keyDown()
{
  return my_special ? SmyDOWN : myDOWN;
}

inline int SDL_ZWin::keyLeft()
{
  return my_special ? SmyLEFT : myLEFT;
}

inline int SDL_ZWin::keyRight()
{
  return my_special ? SmyRIGHT : myRIGHT;
}

bool SDL_ZWin::eventFilter(QObject* o, QEvent* pEvent)
{
    if (QEvent::KeyPress == pEvent->type())
    {
		QueueKey((QKeyEvent*)pEvent, 1);
	    return true;
	} else
    if (QEvent::KeyRelease == pEvent->type())
    {
		QueueKey((QKeyEvent*)pEvent, 0);
	    return true;
	}

    return false;
}

/* Function to translate a keyboard transition and queue the key event
 * This should probably be a table although this method isn't exactly
 * slow.
 */
void SDL_ZWin::QueueKey(QKeyEvent *e, int pressed)
{  
  SDL_keysym keysym;
  int scancode = e->key();

    if(last.scancode)
    {
      // we press/release mod-key without releasing another key
      if(last_mod != my_special){
        SDL_PrivateKeyboard(SDL_RELEASED, &last);
      }
    }

  /* Set the keysym information */
  switch(scancode) 
  {
      case KEYCODE_END:
        scancode = my_special ? SmyRED : myRED;
      break;
      case KEYCODE_CENTER_SELECT:
        scancode = my_special ? SmyCENTER : myCENTER;
      break;
      case KEYCODE_LEFT:
        if (screenRotation == SDL_QT_ROTATION_270) scancode = keyUp();
        else if (screenRotation == SDL_QT_ROTATION_90) scancode = keyDown();
        else scancode = keyLeft();
      break;
      case KEYCODE_UP:
        if (screenRotation == SDL_QT_ROTATION_270) scancode = keyRight();
        else if (screenRotation == SDL_QT_ROTATION_90) scancode = keyLeft();
        else scancode = keyUp();
      break;
      case KEYCODE_RIGHT:
        if (screenRotation == SDL_QT_ROTATION_270) scancode = keyDown();
        else if (screenRotation == SDL_QT_ROTATION_90) scancode = keyUp();
        else scancode = keyRight();
      break;
      case KEYCODE_DOWN:
        if (screenRotation == SDL_QT_ROTATION_270) scancode = keyLeft();
        else if (screenRotation == SDL_QT_ROTATION_90) scancode = keyRight();
        else scancode = keyDown();
      break;
      case KEYCODE_SIDE_SELECT:
        scancode =  my_special ? SmySIDE : mySIDE;
      break;
      case KEYCODE_CARRIER:
        scancode =  my_special ? SmyMUSIC : myMUSIC;
      break;
	  case KEYCODE_CLEAR:
        scancode = my_special ? SmyC : myC;
      break;
	  case KEYCODE_LSK:
        scancode = my_special ? SmyLSOFT : myLSOFT;
      break;
	  case KEYCODE_RSK:
        scancode = my_special ? SmyRSOFT : myRSOFT;
      break;
	  case KEYCODE_SEND:
        scancode = my_special ? SmyCALL : myCALL;
      break;
	  //case KEYCODE_IMAGING:
	  case 0x4021: //Key camera pre pres
        scancode = my_special ? SmyCAMERA : myCAMERA;
      break;
	  case KEYCODE_SIDE_UP:
        scancode = my_special ? SmyVOLUP : myVOLUP; 
      break;
      case KEYCODE_SIDE_DOWN:
        scancode = my_special ? SmyVOLDOWN : myVOLDOWN;
      break;
      case KEYCODE_0:
        scancode = my_special ? Smy0 : my0;
      break;
      case KEYCODE_1:
        scancode = my_special ? Smy1 : my1;
      break;
      case KEYCODE_2:
        scancode = my_special ? Smy2 : my2;
      break;
      case KEYCODE_3:
        scancode = my_special ? Smy3 : my3;
      break;
      case KEYCODE_4:
        scancode = my_special ? Smy4 : my4;
      break;
      case KEYCODE_5:
        scancode = my_special ? Smy5 : my5;
      break;
      case KEYCODE_6:
        scancode = my_special ? Smy6 : my6;
      break;
      case KEYCODE_7: 
        scancode = my_special ? Smy7 : my7;
      break;
      case KEYCODE_8:
        scancode = my_special ? Smy8 : my8;
      break;
      case KEYCODE_9:
        scancode = my_special ? Smy9 : my9;
      break;
      case KEYCODE_STAR:
        scancode = my_special ? SmyASTERISK : myASTERISK;
      break;
      case KEYCODE_POUND:
         scancode = my_special ? SmyNUMERAL : myNUMERAL;
      break;
      
      #ifdef OMEGA_SUPPORT
      case KEYCODE_OMG_TOUCH:
		printf("OMG_TOUCH\n");
		bOmgParse=2;
		scancode = 111;
		break;     
      case KEYCODE_OMG_SCROLL:  
		printf("OMG_SCROLL\n");
		omgScroll(e);
		scancode = 111;
		break;     
      case KEYCODE_OMG_STOP:  
		//bOmgParse=0;
		scancode = 111;
		break;          
      case KEYCODE_OMG_RESUME:  
      printf("OMG_RESUME\n");
      scancode = 111;
      break;     
      case KEYCODE_OMG_RATE:  
      printf("OMG_RATE\n");
      scancode = 111;
      break;    
      #endif
       
      default:
		printf("MAGX: Unknown key 0x%x\n", scancode);
        scancode = SDLK_UNKNOWN;
      break;
  }
  
  if ( scancode == 1234 ) suspend();
  
  if ( scancode == 9999 ) 
  { 
	if (my_special == false) { 
			if(pressed) my_special = true; else my_special = false;
	} else {
			if(pressed) my_special = false; else my_special = true;
	}
	scancode = SDLK_UNKNOWN;       
  } 
  
  keysym.sym = static_cast<SDLKey>(scancode);
  keysym.scancode = scancode;
  keysym.mod = KMOD_NONE;
  if ( SDL_TranslateUNICODE ) {
    QChar qchar = e->text()[0];
    keysym.unicode = qchar.unicode();
  } else {
    keysym.unicode = 0;
  }

  last = keysym;
  last_mod = my_special;

  /* Queue the key event */
  if ( pressed ) {
    SDL_PrivateKeyboard(SDL_PRESSED, &keysym);
  } else 
  {
    last.scancode = 0;
    SDL_PrivateKeyboard(SDL_RELEASED, &keysym);
  }
}

#ifdef OMEGA_SUPPORT
void SDL_ZWin::omgScroll(QKeyEvent *e)
{
	int step = e->step();
	//if ( (bOmgParse==2) && (step != 0) )
	if (step != 0)
	{
		step = (step>0)?1:-1;
		if (screenRotation == SDL_QT_ROTATION_270) 
			my_mouse_pos.setX(my_mouse_pos.x()-step*10);
		if (screenRotation == SDL_QT_ROTATION_90)
			my_mouse_pos.setX(my_mouse_pos.x()+step*10);	
		
		if (my_mouse_pos.x()<0) my_mouse_pos.setX(0);
		if (my_mouse_pos.x()>in_width) my_mouse_pos.setX(in_width);
			
		SDL_PrivateMouseMotion(0, 0, my_mouse_pos.x(), my_mouse_pos.y());
	}
	
	QApplication::setOmegaWheelScrollLines(-step);//e->step() - 
}
#endif

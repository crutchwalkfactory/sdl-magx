#ifndef SDL_FB_IPU
#define SDL_FB_IPU

#include <linux/fb.h>
#include "mxc_pp.h"

//FB&IPU id
extern int fb_dev_fd;
extern int fd_pp;
	
//Info
extern bool isRotate();
extern bool isScalling();
extern bool isBppConvert();

extern struct fb_var_screeninfo fb_orig_vinfo;
extern struct fb_var_screeninfo fb_vinfo;
extern struct fb_fix_screeninfo fb_finfo;
extern pp_buf pp_desc;

#define hw_dbpp (fb_orig_vinfo.bits_per_pixel)
#define vo_dbpp (fb_vinfo.bits_per_pixel)
extern int in_dbpp;

#define bppToPixelSize(a) ((((a)-1)/8)+1)
#define hw_pixel_size bppToPixelSize(hw_dbpp)
#define vo_pixel_size bppToPixelSize(vo_dbpp)
#define in_pixel_size bppToPixelSize(in_dbpp)

#define p_height (fb_orig_vinfo.yres)
#define p_width (fb_orig_vinfo.xres)

#define uIPUMemSize (pp_desc.size)
//Control
extern void preinit();
extern int initFB();
extern int initIPU();
extern int setBppFB( uint32_t in_bpp );
extern int configureIPU( uint32_t width, uint32_t height, uint32_t in_bpp, uint16_t in_rot );
extern int getAllDMAMem();
extern void uninit();
extern void flipPage();
extern bool initDoubleBuffer();
extern void setOriginalBPP(bool org);
extern bool isRotate();
extern bool isScalling();
extern char * pixels();

#endif

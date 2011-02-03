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

///Size of screen
#define p_height (fb_orig_vinfo.yres)
#define p_width (fb_orig_vinfo.xres)

#define uIPUMemSize (pp_desc.size)

//Control

///Setup all the flags and handler of critical situations
extern void preinit();
///Initialize FB
extern int initFB();
///Initialize IPU
extern int initIPU();
///Set bit per pixel to FB
///  in_bpp - bit per pixel (16 or 24)
extern int setBppFB( uint32_t in_bpp );
///Set IPU configuration
///   width  - width video out
///   height - height video out
///   in_bpp - bit per pixel video out
///   in_rot - direction of rotation of the screen (if required)
extern int configureIPU( uint32_t width, uint32_t height, uint32_t in_bpp, uint16_t in_rot );
///Update IPU configuration
extern int reconfigureIPU( uint32_t width, uint32_t height, uint32_t in_bpp, uint16_t in_rot );
///Alloc all memory for further distribution to the needs of libraries and applications
///Warning: you must call before the call configureIPU
extern int getAllDMAMem();
///Unload the video driver. And restores all the values on defaulted.
extern void uninit();
///Displays the contents of the screen
extern void flipPage();
///Initialization of the second buffer, if not used IP transformation
extern bool initDoubleBuffer();
///Restoring the original bit per pixel
///Can be used for example when minimizing applications
extern void setOriginalBPP(bool org);
///Returns a pointer to the input buffer
extern char * pixels();

#endif

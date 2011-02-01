#ifndef SDL_MAGX_VIDEO
#define SDL_MAGX_VIDEO

#include "../SDL_sysvideo.h"

// Hidden "this" pointer for the video functions
#define _THIS	SDL_VideoDevice *_this

// Private display data
struct SDL_PrivateVideoData 
{
	// The main window
	SDL_MainWindow *SDL_MainWin;

	// The fullscreen mode list
	SDL_Rect *SDL_modelist[5][5];

};

// Old variable names
#define SDL_MainWin			(_this->hidden->SDL_MainWin)
#define SDL_modelist		(_this->hidden->SDL_modelist)

#endif

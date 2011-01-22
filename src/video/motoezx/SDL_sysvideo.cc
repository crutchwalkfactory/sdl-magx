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

/* Moto/MAGX based framebuffer implementation with IPU */

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>

#include <zapplication.h>

#include "SDL_timer.h"
#include "SDL_QWin.h"

extern "C" {

#include "../SDL_sysvideo.h"
#include "../../events/SDL_events_c.h"
#include "../SDL_pixels_c.h"
#include "SDL_sysevents_c.h"
#include "SDL_sysmouse_c.h"
#include "SDL_syswm_c.h"
#include "SDL_lowvideo.h"

	// Initialization/Query functions
	static int MAGX_VideoInit(_THIS, SDL_PixelFormat *vformat);
	static SDL_Rect **MAGX_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags);
	static SDL_Surface *MAGX_SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags);
	static void MAGX_UpdateMouse(_THIS);
	static int MAGX_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors);
	static void MAGX_VideoQuit(_THIS);

	static void MAGX_NormalUpdate(_THIS, int numrects, SDL_Rect *rects);

	// Hardware surface functions (now not uses)
	static int MAGX_AllocHWSurface(_THIS, SDL_Surface *surface);
	static int MAGX_LockHWSurface(_THIS, SDL_Surface *surface);
	static void MAGX_UnlockHWSurface(_THIS, SDL_Surface *surface);
	static void MAGX_FreeHWSurface(_THIS, SDL_Surface *surface);
	static int MAGX_FlipHWSurface(_THIS, SDL_Surface *surface);


	// FB driver bootstrap functions

	static int MAGX_Available(void)
	{
		return(1);
	}

	static void MAGX_DeleteDevice(SDL_VideoDevice *device)
	{
		SDL_free(device->hidden);
		SDL_free(device);
	}


	static SDL_VideoDevice *MAGX_CreateDevice(int devindex)
	{
		SDL_VideoDevice *device;

		// Initialize all variables that we clean on shutdown
		device = (SDL_VideoDevice *)SDL_malloc(sizeof(SDL_VideoDevice));
		if ( device ) 
		{
			SDL_memset(device, 0, (sizeof *device));
			device->hidden = (struct SDL_PrivateVideoData *)
			SDL_malloc((sizeof *device->hidden));
		}
		if ( (device == NULL) || (device->hidden == NULL) ) 
		{
			SDL_OutOfMemory();
			if ( device )
				SDL_free(device);
			return(0);
		}
		SDL_memset(device->hidden, 0, (sizeof *device->hidden));

		// Set the function pointers 
		device->VideoInit = MAGX_VideoInit;
		device->ListModes = MAGX_ListModes;
		device->SetVideoMode = MAGX_SetVideoMode;
		device->UpdateMouse = MAGX_UpdateMouse;
		device->SetColors = MAGX_SetColors;
		device->UpdateRects = NULL;
		device->VideoQuit = MAGX_VideoQuit;
		device->AllocHWSurface = MAGX_AllocHWSurface;
		device->CheckHWBlit = NULL;
		device->FillHWRect = NULL;
		device->SetHWColorKey = NULL;
		device->SetHWAlpha = NULL;
		device->LockHWSurface = MAGX_LockHWSurface;
		device->UnlockHWSurface = MAGX_UnlockHWSurface;
		device->FlipHWSurface = MAGX_FlipHWSurface;
		device->FreeHWSurface = MAGX_FreeHWSurface;
		device->SetIcon = NULL;
		device->SetCaption = MAGX_SetWMCaption;
		device->IconifyWindow = MAGX_IconifyWindow;
		device->GrabInput = MAGX_GrabInput;
		device->GetWMInfo = NULL;
		device->FreeWMCursor = MAGX_FreeWMCursor;
		device->CreateWMCursor = MAGX_CreateWMCursor;
		device->ShowWMCursor = MAGX_ShowWMCursor;
		device->WarpWMCursor = MAGX_WarpWMCursor;
		device->InitOSKeymap = MAGX_InitOSKeymap;
		device->PumpEvents = MAGX_PumpEvents;

		device->free = MAGX_DeleteDevice;
		device->ToggleFullScreen = MAGX_ToggleFullScreen;

		return device;
	}

	VideoBootStrap EzX_bootstrap = {
	"MAGX", "Motorola / MAGX FB+IPU graphics",
	MAGX_Available, MAGX_CreateDevice
	};

	// Yes, this isn't the fastest it could be, but it works nicely
	static int MAGX_AddMode(_THIS, int index, unsigned int w, unsigned int h)
	{
		SDL_Rect *mode;
		int i;
		int next_mode;

		// Check to see if we already have this mode
		if ( SDL_nummodes[index] > 0 ) 
		{
			for ( i=SDL_nummodes[index]-1; i >= 0; --i ) 
			{
				mode = SDL_modelist[index][i];
				if ( (mode->w == w) && (mode->h == h) ) 
					return(0);
			}
		}

		// Set up the new video mode rectangle
		mode = (SDL_Rect *)SDL_malloc(sizeof *mode);
		if ( mode == NULL ) 
		{
			SDL_OutOfMemory();
			return(-1);
		}
		mode->x = 0;
		mode->y = 0;
		mode->w = w;
		mode->h = h;

		// Allocate the new list of modes, and fill in the new mode
		next_mode = SDL_nummodes[index];
		SDL_modelist[index] = (SDL_Rect **)
		SDL_realloc(SDL_modelist[index], (1+next_mode+1)*sizeof(SDL_Rect *));
		if ( SDL_modelist[index] == NULL ) 
		{
			SDL_OutOfMemory();
			SDL_nummodes[index] = 0;
			SDL_free(mode);
			return(-1);
		}
		SDL_modelist[index][next_mode] = mode;
		SDL_modelist[index][next_mode+1] = NULL;
		SDL_nummodes[index]++;

		return(0);
	}

	int MAGX_VideoInit(_THIS, SDL_PixelFormat *vformat)
	{
		// Determine the screen depth
		vformat->BitsPerPixel = 16; //Default screen bpp 24, best 16

		//Add video mode to list
		QSize desktop_size = qApp->desktop()->size();   
		int bpp = ((vformat->BitsPerPixel+7)/8)-1;
		//Normal mode
		MAGX_AddMode(_this, bpp, desktop_size.width(), desktop_size.height());
		MAGX_AddMode(_this, bpp, desktop_size.height(), desktop_size.width());
		//Mode with use IPU
		MAGX_AddMode(_this, bpp, desktop_size.width()*2, desktop_size.height()*2);
		MAGX_AddMode(_this, bpp, desktop_size.height()*2, desktop_size.width()*2);		

		// Determine the current screen size
		_this->info.current_w = desktop_size.width();
		_this->info.current_h = desktop_size.height();

		// Create the window / widget
		SDL_Win = new SDL_ZWin(desktop_size);
		if( !SDL_Win->isOK() )
		{
			delete SDL_Win;
			SDL_Win = 0;
			return -1;
		}
		qApp->setMainWidget(SDL_Win);
		SDL_Win->show();

		// Fill in some window manager capabilities
		_this->info.wm_available = 0;

		// We're done!
		return(0);
	}

	// We support any dimension at our bit-depth
	SDL_Rect **MAGX_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags)
	{
		SDL_Rect **modes;
		modes = ((SDL_Rect **)0);
		if ( (flags & SDL_FULLSCREEN) == SDL_FULLSCREEN ) 
			modes = SDL_modelist[((format->BitsPerPixel+7)/8)-1];
		else 
			if ( format->BitsPerPixel == _this->screen->format->BitsPerPixel )
				modes = ((SDL_Rect **)-1);
		return(modes);
	}

	/* FIXME: check return values and cleanup here */
	SDL_Surface *MAGX_SetVideoMode(_THIS, SDL_Surface *current,
							int width, int height, int bpp, Uint32 flags)
	{
		printf("MAGX: SetVideoMode width=%d height=%d bpp=%d\n", width, height, bpp);
		
		if ( flags & SDL_OPENGL ) 
		{
			SDL_SetError("OpenGL not supported");
			return(NULL);
		} 
		
		if ( (width>640 && height>480) || (width>480 && height>640) || (bpp<16) )
		{
			SDL_SetError("Not supportet video mode!");
			return(NULL);		
		}
		
		if ( flags & SDL_HWSURFACE ) 
			printf("SDL request HW!\n");

		if ( !SDL_Win->SetVideoMode(width, height, bpp) )
		{
			SDL_SetError("Cannot init IPU!");
			return(NULL);
		} 

		current->flags = SDL_FULLSCREEN;
		current->w = width;
		current->h = height;
		current->pitch = SDL_CalculatePitch(current);
		current->pixels = SDL_Win->getFBBuf();

		_this->UpdateRects = MAGX_NormalUpdate;
		
		// We're done
		return(current);
	}

	/* Update the current mouse state and position */
	void MAGX_UpdateMouse(_THIS)
	{
		QPoint point(-1, -1);
		if ( SDL_Win->isActiveWindow() ) 
		{
			point = SDL_Win->mousePos();
		}

		if ( (point.x() >= 0) && (point.x() < SDL_VideoSurface->w) &&
		(point.y() >= 0) && (point.y() < SDL_VideoSurface->h) ) 
		{
			SDL_PrivateAppActive(1, SDL_APPMOUSEFOCUS);
			SDL_PrivateMouseMotion(0, 0,
			(Sint16)point.x(), (Sint16)point.y());
		} else 
		{
			SDL_PrivateAppActive(0, SDL_APPMOUSEFOCUS);
		}
	}

	// We don't actually allow hardware surfaces other than the main one
	static int MAGX_AllocHWSurface(_THIS, SDL_Surface *surface)
	{
		return(-1);
	}
	static void MAGX_FreeHWSurface(_THIS, SDL_Surface *surface)
	{
		return;
	}
	static int MAGX_LockHWSurface(_THIS, SDL_Surface *surface)
	{
		return(0);
	}
	static void MAGX_UnlockHWSurface(_THIS, SDL_Surface *surface)
	{
		return;
	}
	static int MAGX_FlipHWSurface(_THIS, SDL_Surface *surface)
	{
	}
	
	// Various screen update functions available
	static void MAGX_NormalUpdate(_THIS, int numrects, SDL_Rect *rects)
	{
		SDL_Win->flipScreen();
	}
	
	// Is the system palette settable?
	int MAGX_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors)
	{
		return -1;
	}

	void MAGX_VideoQuit(_THIS)
	{
		// This is dumb, but if I free this, the app doesn't exit correctly.
		// Of course, this will leak memory if init video is done more than once.
		// Sucks but such is life.

		SDL_Win->uninitVideo();
		_this->screen->pixels = NULL;
		delete SDL_Win;
		SDL_Win = 0;
	}
}; /* Extern C */

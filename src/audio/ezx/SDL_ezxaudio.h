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

#ifndef _SDL_ezxaudio_h
#define _SDL_ezxaudio_h

#include "ezxsound.h"

#include "../SDL_sysaudio.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	SDL_AudioDevice *thisdevice

//#define HW_AO_OUT

struct SDL_PrivateAudioData 
{
	/* The file descriptor for the audio device */
	Uint8 *mixbuf;
	Uint32 mixlen;
	
	#ifndef HW_AO_OUT
	AM_NORMAL_DEV_INTERFACE * audio_dev;
	#else
	AM_HW_DEV_INTERFACE * audio_dev;
	#endif
};

#endif /* _SDL_ezxaudio_h */

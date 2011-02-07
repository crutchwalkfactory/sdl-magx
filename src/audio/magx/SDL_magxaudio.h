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

#ifndef SDL_MAGXAUDIO_H
#define SDL_MAGXAUDIO_H

#include "AAL_audio.h"

#include "../SDL_sysaudio.h"

/* Hidden "this" pointer for the video functions */
#define _THIS	SDL_AudioDevice *_this

struct SDL_PrivateAudioData 
{
	/* The file descriptor for the audio device */
	AAL_BUF aalBuf;
	
    float frame_ticks;
    float next_frame;
};

#define FUDGE_TICKS	10	/* The scheduler overhead ticks per frame */

#define aalBuf			(_this->hidden->aalBuf)
#define frame_ticks		(_this->hidden->frame_ticks)
#define next_frame		(_this->hidden->next_frame)
#define buf_size		(_this->spec.size)

#endif /* _SDL_ezxaudio_h */

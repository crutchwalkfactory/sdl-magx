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

    This file written by Prozorov Anton <prozanton@gmail.com>
*/
#include "SDL_config.h"

/* 	Output audio via libezxsound.so using interface AAL for Motorola MAGX Phone */
 
#include "SDL_rwops.h"
#include "SDL_timer.h"
#include "SDL_audio.h"
#include "../SDL_audiomem.h"
#include "../SDL_audio_c.h"
#include "../SDL_audiodev_c.h"
#include "SDL_magxaudio.h"

// MAGX include
#include <ZConfig.h>
#include <ZApplication.h>

static int bAutoCloseAudio;
static int id;
static AAL_PARAM config;

extern "C"
{

void suspendAudio()
{
	if ( bAutoCloseAudio && id!=0  )
	{
		AAL_close(id);
		AAL_sync(id);
		id=0;
	}
}

void resumeAudio()
{
	if ( bAutoCloseAudio && id==0 )	
	{
		id = AAL_open( &config );
		AAL_sync(id);		
	}
}

}

/* Audio driver functions */
static SDL_AudioDevice *MAGXAudio_CreateDevice(int devindex);
static int MAGXAudio_OpenAudio(_THIS, SDL_AudioSpec *spec);
static void MAGXAudio_WaitAudio(_THIS);
static void MAGXAudio_PlayAudio(_THIS);
static Uint8 *MAGXAudio_GetAudioBuf(_THIS);
static void MAGXAudio_CloseAudio(_THIS);
static void MAGXAudio_DeleteDevice(SDL_AudioDevice *device);

/* Audio driver bootstrap functions */
static int MAGXAudio_Available(void)
{
	return( 1 );
}

AudioBootStrap MAGXAudio_bootstrap = {
	"magx", "SDL MAGX audio driver",
	MAGXAudio_Available, MAGXAudio_CreateDevice
};

static SDL_AudioDevice *MAGXAudio_CreateDevice(int devindex)
{
	SDL_AudioDevice *thisdevice;

	/* Initialize all variables that we clean on shutdown */
	thisdevice = (SDL_AudioDevice *)SDL_malloc(sizeof(SDL_AudioDevice));
	if ( thisdevice ) {
		SDL_memset(thisdevice, 0, (sizeof *thisdevice));
		thisdevice->hidden = (struct SDL_PrivateAudioData *)
				SDL_malloc((sizeof *thisdevice->hidden));
	}
	if ( (thisdevice == NULL) || (thisdevice->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( thisdevice ) {
			SDL_free(thisdevice);
		}
		return(0);
	}
	SDL_memset(thisdevice->hidden, 0, (sizeof *thisdevice->hidden));

	/* Set the function pointers */
	thisdevice->OpenAudio = MAGXAudio_OpenAudio;
	thisdevice->WaitAudio = MAGXAudio_WaitAudio;
	thisdevice->PlayAudio = MAGXAudio_PlayAudio;
	thisdevice->GetAudioBuf = MAGXAudio_GetAudioBuf;
	thisdevice->CloseAudio = MAGXAudio_CloseAudio;
	thisdevice->free = MAGXAudio_DeleteDevice;

	return thisdevice;
}

static int MAGXAudio_OpenAudio(_THIS, SDL_AudioSpec *spec)
{
	printf("\nMAGX_AO: init with freq=%d channels=%d bps=%d samples=%d\n",
				spec->freq,spec->channels,(spec->format&0x00FF)/8,spec->samples);
	
	//Read settings from file 
	ZConfig mySDL(QString(qApp->argv()[0])+"_SDL.cfg", false);
	bAutoCloseAudio = mySDL.readBoolEntry("AUDIO", "AutoCloseAudio", true);
	int volume = mySDL.readNumEntry("AUDIO", "Volume", 4);
	int synchronization = mySDL.readNumEntry("AUDIO", "Synchronization", 1);
	
	//Check support frequence
	unsigned int uFreqList[9]={8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000};
	int i=0;
	for ( i=0; i<9; i++ )
		if ( uFreqList[i]==spec->freq )
			break;
	if ( i>=9 )
	{
		spec->freq = 44100;
		i=7;
		printf("MAGX_AO: change audio frequence!\n");
	}	
	
	//Configure AO
	memset(&config, 0, sizeof(AAL_PARAM));
	config.d2 = 0x4080803;
	config.d1 = 3;
	
	config.chanel = spec->channels-1;
	config.vol = volume*100/7;
	config.freq = spec->freq;
	config.bps = (spec->format&0x00FF)/8;

	//Open AO
	if ( volume>0 )
	{
		id = AAL_open( &config );
		AAL_sync(id);
	} else
		id=0;

	printf("MAGX_AO: Sample %d Size %d\n",spec->samples,spec->size);

	//Allocate mixing buffer
	aalBuf.length = spec->size;
	aalBuf.buffer = (void *) SDL_AllocAudioMem(aalBuf.length);
	if ( aalBuf.buffer == NULL )
	{
		return(-1);
	}
	SDL_memset(aalBuf.buffer, spec->silence, spec->size);

	//For audio synchronization
	if ( synchronization )
	{
		frame_ticks = (float)(spec->samples*1000)/spec->freq;
		next_frame = SDL_GetTicks()+frame_ticks;
	} else
		frame_ticks=0;
	
	return(0);
}

static void MAGXAudio_WaitAudio(_THIS)
{
	// See if we need to use timed audio synchronization
	if ( frame_ticks ) 
	{
		// Use timer for general audio synchronization
		int ticks;

		ticks = (int)(next_frame - SDL_GetTicks())-FUDGE_TICKS;
		if ( ticks > 0 ) 
			SDL_Delay(ticks);
	}
}

static void MAGXAudio_PlayAudio(_THIS)
{
	// If timer synchronization is enabled, set the next write frame
	if ( frame_ticks )
		next_frame += frame_ticks;		
	
	if ( id==0 || AAL_in_call_capture(id) )
		return;
	
	int i;
	if ( frame_ticks )
	{
		for (i=1; i<buf_size/4; i++ )
			if ( ((int*)aalBuf.buffer)[i]==0 && ((int*)aalBuf.buffer)[i-1]==0 && ((int*)aalBuf.buffer)[i-2]==0 )
				break;

		aalBuf.length=i*4;
	}
	AAL_write(id, &aalBuf, &i);

	if ( i!=0 )
	{
		printf("MAGX_AO: Error write!\n");
	}
}

static Uint8 *MAGXAudio_GetAudioBuf(_THIS)
{
	return (Uint8*)aalBuf.buffer;
}

static void MAGXAudio_CloseAudio(_THIS)
{
	if ( aalBuf.buffer != NULL ) 
	{
		SDL_FreeAudioMem(aalBuf.buffer);
		aalBuf.buffer = NULL;
	}
	if ( id )
	{
		AAL_close(id);
		AAL_sync(id);
		id=0;
	}
}

static void MAGXAudio_DeleteDevice(SDL_AudioDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}


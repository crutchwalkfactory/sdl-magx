#include "SDL_config.h"

/* Moto/MAGX based framebuffer implementation with IPU */

#include <sys/mman.h>
#include <unistd.h>

#include <ZApplication.h>

#include "SDL_timer.h"
#include "SDL_magx_win.h"
#include "SDL_magx_kernel.h"
#include "SDL_magx_logo.c"
#include "ipu_alloc.h"

#if 0
#define DebugFunction() printf("MAGX_VO: video - %s()\n",__FUNCTION__) 
#else
#define DebugFunction()
#endif

extern "C" 
{

	#include "../SDL_sysvideo.h"
	#include "../../events/SDL_events_c.h"
	#include "../SDL_pixels_c.h"

	#include "SDL_magx_sys.h"
	#include "SDL_magx_video.h"

	// Initialization/Query functions
	static int MAGX_VideoInit(_THIS, SDL_PixelFormat *vformat);
	static SDL_Rect **MAGX_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags);
	static SDL_Surface *MAGX_SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags);
	static void MAGX_UpdateMouse(_THIS);
	static int MAGX_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors);
	static void MAGX_VideoQuit(_THIS);

	static void MAGX_NormalUpdate(_THIS, int numrects, SDL_Rect *rects);
	static void MAGX_NoUpdate(_THIS, int numrects, SDL_Rect *rects){};

	// Hardware surface functions
	static int MAGX_AllocHWSurface(_THIS, SDL_Surface *surface);
	static int MAGX_LockHWSurface(_THIS, SDL_Surface *surface);
	static void MAGX_UnlockHWSurface(_THIS, SDL_Surface *surface);
	static void MAGX_FreeHWSurface(_THIS, SDL_Surface *surface);
	static int MAGX_FlipHWSurface(_THIS, SDL_Surface *surface);

	static void MAGX_FreeHWSurfaces(_THIS);
	static int MAGX_InitHWSurfaces(_THIS, SDL_Surface *screen, char *base, int size);

	// FB driver bootstrap functions
	static int MAGX_Available(void)
	{
		DebugFunction();
		
		return 1;
	}

	static void MAGX_DeleteDevice(SDL_VideoDevice *device)
	{
		DebugFunction();
		
		SDL_free(device->hidden);
		SDL_free(device);
	}

	static SDL_VideoDevice *MAGX_CreateDevice(int devindex)
	{
		DebugFunction();
		
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
		device->VideoQuit = MAGX_VideoQuit;	
		device->ListModes = MAGX_ListModes;
		device->SetVideoMode = MAGX_SetVideoMode;
		device->UpdateMouse = MAGX_UpdateMouse;
		device->SetColors = MAGX_SetColors;
		device->UpdateRects = NULL;
		device->InitOSKeymap = MAGX_InitOSKeymap;
		device->PumpEvents = MAGX_PumpEvents;
		
		//Overlay
		device->CreateYUVOverlay = NULL;

		//WM
		device->SetIcon = NULL;
		device->SetCaption = MAGX_SetWMCaption;
		device->IconifyWindow = MAGX_IconifyWindow;
		device->GrabInput = MAGX_GrabInput;
		device->GetWMInfo = NULL;
		
		//Cursor
		device->FreeWMCursor = MAGX_FreeWMCursor;
		device->CreateWMCursor = MAGX_CreateWMCursor;
		device->ShowWMCursor = MAGX_ShowWMCursor;
		device->WarpWMCursor = MAGX_WarpWMCursor;

		//HW
		device->CheckHWBlit = NULL;
		device->FillHWRect = NULL;

		device->SetHWColorKey = NULL;
		device->SetHWAlpha = NULL;

		device->AllocHWSurface = MAGX_AllocHWSurface;		
		device->FreeHWSurface = MAGX_FreeHWSurface;
		device->LockHWSurface = MAGX_LockHWSurface;
		device->UnlockHWSurface = MAGX_UnlockHWSurface;
		device->FlipHWSurface = MAGX_FlipHWSurface;	
		
		device->free = MAGX_DeleteDevice;
		device->ToggleFullScreen = MAGX_ToggleFullScreen;

		return device;
	}

	VideoBootStrap MAGX_bootstrap = {
	"MAGX", "Motorola / MAGX FB+IPU graphics",
	MAGX_Available, MAGX_CreateDevice
	};
	
	int MAGX_VideoInit(_THIS, SDL_PixelFormat *vformat)
	{
		DebugFunction();
		
		// Create the window
		SDL_MainWin = new SDL_MainWindow();
		qApp->setMainWidget(SDL_MainWin);
		SDL_MainWin->show();

		//Init graphic out
		preinit();
		if ( !initFB() )
		{
			SDL_SetError("Can not init FB!");
			return -1;
		}
		if ( !initIPU() )
		{
			SDL_SetError("Can not init FB!");
			return -1;
		}
		if ( !getAllDMAMem() )
		{
			SDL_SetError("DMA memory overload!");
			return -1;			
		}
		
		// Show logo
		// logo: 230x150 2bpp
		setBppFB( 16 );
		memset(pixels(), 255, p_height*p_width*2);
		char * pCur = pixels()+(p_height-150)*p_width+(p_width-230);
		char * pLogo = (char*)sdlLogo+230*149*2;
		for ( int i=0; i<150; i++ )
		{
			memcpy(pCur, pLogo, 2*230);
			pLogo -= 2*230;
			pCur += 2*p_width;
		}
		sleep(1);
			
		// Determine the screen depth
		vformat->BitsPerPixel = 16; //Default screen bpp 24, best 16
		vformat->BytesPerPixel = 2;

		// Fill in our hardware acceleration capabilities
		_this->info.current_w = p_width;
		_this->info.current_h = p_height;
		_this->info.video_mem = uIPUMemSize/1024;
		_this->info.wm_available = 1;
		_this->info.hw_available = 1;
		
		// Fill video mode list
		for ( int i=0; i<3; i++ )
			SDL_modelist[(i<2)?i:4][0]=NULL;
		for( int i=2; i<4; i++)
		{
			//Normal mode
			SDL_modelist[i][0]=(SDL_Rect*)SDL_malloc(sizeof(SDL_Rect));
			SDL_modelist[i][0]->x=0; SDL_modelist[i][0]->y=0;
			SDL_modelist[i][0]->w=p_width,SDL_modelist[i][0]->h=p_height;

			//Rotate mode
			SDL_modelist[i][1]=(SDL_Rect*)SDL_malloc(sizeof(SDL_Rect));
			SDL_modelist[i][1]->x=0; SDL_modelist[i][1]->y=0;
			SDL_modelist[i][1]->w=p_height,SDL_modelist[i][1]->h=p_width;

			//Downscale mode
			SDL_modelist[i][2]=(SDL_Rect*)SDL_malloc(sizeof(SDL_Rect));
			SDL_modelist[i][2]->x=0; SDL_modelist[i][2]->y=0;
			SDL_modelist[i][2]->w=p_width*2,SDL_modelist[i][2]->h=p_height*2;

			//Downscale and rotate mode
			SDL_modelist[i][3]=(SDL_Rect*)SDL_malloc(sizeof(SDL_Rect));
			SDL_modelist[i][3]->x=0; SDL_modelist[i][3]->y=0;
			SDL_modelist[i][3]->w=p_height*2,SDL_modelist[i][3]->h=p_width*2;

			SDL_modelist[i][4]=NULL;
		}

		// We're done!
		return 0;
	}

	// We support any dimension at our bit-depth
	SDL_Rect **MAGX_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags)
	{
		DebugFunction();
		
		if(format->BitsPerPixel==15)
			return SDL_modelist[4];
		else
			return SDL_modelist[bppToPixelSize(format->BitsPerPixel)];
	}
	
	/* FIXME: check return values and cleanup here */
	SDL_Surface *MAGX_SetVideoMode(_THIS, SDL_Surface *current,
							int width, int height, int bpp, Uint32 flags)
	{
		printf("MAGX_VO: SetVideoMode width=%d height=%d bpp=%d\n", width, height, bpp);
	
		if ( flags&SDL_OPENGL ) 
		{
			SDL_SetError("OpenGL not supported");
			return(NULL);
		} 
		
		if ( (width>640 && height>480) || (width>480 && height>640) || (bpp<16) )
		{
			SDL_SetError("No supportet video mode!");
			return(NULL);		
		}

		static int setVideoMode=1;
		
		if ( setVideoMode )
		{
			//set video mode
			if ( bpp >= 24 )
				printf("MAGX: Warning! Not recommended use bpp=%d, it very slow!\n", bpp);
			setBppFB(bpp);
			if ( !configureIPU(width, height, bpp, screenRotation) )
			{
				SDL_SetError("No configure IPU!");
				return(NULL);				
			}
			if ( flags&SDL_DOUBLEBUF )
				initDoubleBuffer();
			
			setVideoMode=0;
		} else
		{
			//change video mode
			setBppFB(bpp);
			if ( !reconfigureIPU(width, height, bpp, screenRotation) )
			{
				SDL_SetError("No reconfigure IPU!");
				return(NULL);				
			}
			if ( flags&SDL_DOUBLEBUF )
				initDoubleBuffer();
		}

		current->flags = SDL_FULLSCREEN | SDL_PREALLOC | (flags&SDL_DOUBLEBUF);
		current->w = width;
		current->h = height;
		current->pitch = SDL_CalculatePitch(current);
		current->pixels = pixels();
		
		int mode = (isRotate()?1:0) + (isScalling()?1:0) + (isBppConvert()?1:0);
		
		if ( isRotate() )
			SDL_MainWin->setRotation(screenRotation);
		else
			SDL_MainWin->setRotation(SDL_QT_NO_ROTATION);		
		
		if ( mode<2 )
			current->flags |= SDL_HWSURFACE;
		else
			current->flags |= SDL_SWSURFACE;
			
		if ( flags&SDL_DOUBLEBUF || mode==0 )	
			_this->UpdateRects = MAGX_NoUpdate;
		else
			_this->UpdateRects = MAGX_NormalUpdate;

		printf("MAGX: SDL request:\n");
		if ( flags & SDL_HWSURFACE ) 
			printf("\t- Surface is in video memory (%c);\n", ((current->flags&SDL_HWSURFACE)?'+':' '));
		if ( flags & SDL_HWACCEL ) 
			printf("\t- Hardware acceleration (%c);\n", ((current->flags&SDL_HWACCEL)?'+':' '));			
		if ( flags & SDL_DOUBLEBUF ) 
			printf("\t- Double-buffered video mode (%c);\n", ((current->flags&SDL_DOUBLEBUF)?'+':' '));	
		
		// We're done
		return(current);
	}

	// Update the current mouse state and position
	void MAGX_UpdateMouse(_THIS)
	{
		DebugFunction();
		
		int x, y;
		getMousPos( x, y );
		
		SDL_PrivateAppActive(1, SDL_APPMOUSEFOCUS);
		SDL_PrivateMouseMotion(0, 0, (Sint16)x, (Sint16)y);
	}

	// We don't actually allow hardware surfaces other than the main one
	static int MAGX_AllocHWSurface(_THIS, SDL_Surface *surface)
	{
		printf("MAGX: MAGX_AllocHWSurface\n");
		
		if ( surface->pitch > SDL_VideoSurface->pitch ) 
		{
			SDL_SetError("Surface requested wider than screen");
			return(-1);
		}
		
		surface->pitch = SDL_CalculatePitch(surface);

		unsigned int size = surface->h * surface->pitch;
		
		unsigned int addr = ipu_malloc(size);
		if ( addr==0 )
		{
			SDL_SetError("Video memory too fragmented");
			return -1;
		}
		
		printf("MAGX: Allocated %u bytes at %u\n", size, addr);
		
		surface->flags |= SDL_HWSURFACE | SDL_PREALLOC;
		surface->pixels = (char*)mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_pp, addr);
		if (surface->pixels == MAP_FAILED) 
		{
			SDL_SetError("No mmap memory");
			return -1;
		}
		surface->hwdata = (struct private_hwdata *)addr;
		return 0;
	}
	
	static void MAGX_FreeHWSurface(_THIS, SDL_Surface *surface)
	{
		printf("MAGX: MAGX_FreeHWSurface\n");
		
		munmap(surface->pixels, surface->h*surface->pitch);
		
		unsigned int addr = (unsigned int)surface->hwdata;
		ipu_free(addr);

		surface->pixels = NULL;
		surface->hwdata = NULL;
	}

	static int MAGX_LockHWSurface(_THIS, SDL_Surface *surface)
	{
		return(0);
	}
	
	static void MAGX_UnlockHWSurface(_THIS, SDL_Surface *surface)
	{
	}
	
	static int SkipedFirstFlip=0;
	
	static int MAGX_FlipHWSurface(_THIS, SDL_Surface *surface)
	{
		DebugFunction();
		
		if ( SkipedFirstFlip )
			flipPage();
		else
			SkipedFirstFlip=1;
	}
	
	// Various screen update functions available
	static void MAGX_NormalUpdate(_THIS, int numrects, SDL_Rect *rects)
	{
		DebugFunction();
		if ( SkipedFirstFlip )
			flipPage();
		else
			SkipedFirstFlip=1;
	}
	
	int MAGX_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors)
	{
		DebugFunction();
		
		return -1;
	}

	void MAGX_VideoQuit(_THIS)
	{
		printf("MAGX: MAGX_VideoQuit\n");
		
		uninit();
		
		_this->screen->pixels = NULL;
		
		delete SDL_MainWin;
		SDL_MainWin = NULL;
	}

}; /* Extern C */

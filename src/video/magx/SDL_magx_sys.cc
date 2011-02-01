#include "SDL_config.h"
#include "SDL_mouse.h"

#include <unistd.h>

#include "SDL_magx_win.h"

extern "C" 
{
	#include "../../events/SDL_events_c.h"
	
	#include "SDL_magx_sys.h"

	void MAGX_SetWMCaption(_THIS, const char *title, const char *icon)
	{
		SDL_MainWin->setCaption(title);
	}

	int MAGX_IconifyWindow(_THIS) 
	{
		SDL_MainWin->suspend();
		return true;
	}

	int MAGX_ToggleFullScreen(_THIS, int fullscreen)
	{
		return -1;
	}

	SDL_GrabMode MAGX_GrabInput(_THIS, SDL_GrabMode mode) 
	{
		return mode;
	}

	struct WMcursor 
	{
		char *bits;
	};

	WMcursor *MAGX_CreateWMCursor(_THIS,
			Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y)
	{
		static WMcursor dummy;
		dummy.bits = 0;
		return &dummy;
	}

	int MAGX_ShowWMCursor(_THIS, WMcursor *cursor)
	{
		return 1;
	}

	void MAGX_FreeWMCursor(_THIS, WMcursor *cursor)
	{
	}

	void MAGX_WarpWMCursor(_THIS, Uint16 x, Uint16 y)
	{
		SDL_MainWin->setMousePos(QPoint(x, y));
		SDL_PrivateMouseMotion(0, 0, x, y);
	}

	extern int my_suspended;
	extern int my_focus;
	extern void suspendAudio();
	extern void resumeAudio();

	void MAGX_PumpEvents(_THIS)
	{
		if( !qApp ) 
			return; 
		qApp->processEvents();	
		if ( !my_focus )
		{
			printf("MAGX_VO: Wait focus in...\n");
			while ( !my_focus && !my_suspended )
			{
				qApp->processEvents();
				sleep(1);
			}
		}
		if ( my_suspended )	
		{	
			printf("MAGX_VO: Wait end suspend...\n");
			suspendAudio();
			while ( my_suspended )
			{
				qApp->processEvents();	
				sleep(1);
			}
			resumeAudio();
			printf("MAGX_VO: End suspend\n");
		}
	}

	void MAGX_InitOSKeymap(_THIS)
	{
	}

}; /* Extern C */

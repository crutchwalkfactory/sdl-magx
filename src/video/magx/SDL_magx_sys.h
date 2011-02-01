#include "SDL_config.h"

#include "SDL_magx_video.h"

//WM
extern void MAGX_SetWMCaption(_THIS, const char *title, const char *icon);
extern int MAGX_IconifyWindow(_THIS);
extern int MAGX_ToggleFullScreen(_THIS, int fullscreen);
extern SDL_GrabMode MAGX_GrabInput(_THIS, SDL_GrabMode mode);

//Mouse
extern void MAGX_FreeWMCursor(_THIS, WMcursor *cursor);
extern WMcursor *MAGX_CreateWMCursor(_THIS,
		Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y);
extern int MAGX_ShowWMCursor(_THIS, WMcursor *cursor);
extern void MAGX_WarpWMCursor(_THIS, Uint16 x, Uint16 y);

//Events
extern void MAGX_InitOSKeymap(_THIS);
extern void MAGX_PumpEvents(_THIS);


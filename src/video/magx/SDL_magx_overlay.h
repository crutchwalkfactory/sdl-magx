#include "SDL_config.h"

#include "SDL_video.h"
#include "../SDL_sysvideo.h"

extern SDL_Overlay *SDL_CreateYUV_HW(_THIS, int width, int height, Uint32 format, SDL_Surface *display);
extern int SDL_LockYUV_HW(_THIS, SDL_Overlay *overlay);
extern void SDL_UnlockYUV_HW(_THIS, SDL_Overlay *overlay);
extern int SDL_DisplayYUV_HW(_THIS, SDL_Overlay *overlay, SDL_Rect *src, SDL_Rect *dst);
extern void SDL_FreeYUV_HW(_THIS, SDL_Overlay *overlay);

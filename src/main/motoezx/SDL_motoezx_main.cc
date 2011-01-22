
/* Include the SDL main definition header */
#include "SDL_main.h"
#include <stdlib.h>
#include <unistd.h>
#ifdef main
#undef main
#endif
#include <zapplication.h>
#include <stdlib.h>

extern int SDL_main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
  ZApplication a(argc, argv);
  int r = SDL_main(argc, argv);
  return r;
}

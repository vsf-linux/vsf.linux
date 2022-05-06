/*
    SDL_vsf_main.c, placed in the public domain by SimonQian  5/5/2022

    The WinMain function -- calls your program's main() function
*/
#include "SDL_config.h"

#ifdef __VSF__

/* Include the SDL main definition header */
#include "SDL.h"
#include "SDL_main.h"

#ifdef main
#  undef main
#endif /* main */

int main(int argc, char *argv[])
{
    SDL_SetMainReady();

    /* Run the application main() code */
    return SDL_main(argc, argv);
}

#endif /* __VSF__ */

/* vi: set ts=4 sw=4 expandtab: */

#include "../raw/src/SDL_internal.h"

#ifdef __VSF__

NO_INIT SDL_VSF_platform SDL_platform;

void vsf_sdl2_init(vsf_sdl2_cfg_t *cfg)
{
    SDL_platform.cfg = *cfg;
    SDL_platform.is_disp_inited = false;
    SDL_platform.is_audio_inited = false;
}

#endif
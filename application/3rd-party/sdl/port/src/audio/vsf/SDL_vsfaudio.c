#include "../../../../raw/src/SDL_internal.h"

#if SDL_AUDIO_DRIVER_VSF

#include "SDL_audio.h"
#include "../../../../raw/src/audio/SDL_sysaudio.h"

#include "vsf.h"

static SDL_bool
VSFAudio_Init(SDL_AudioDriverImpl * impl)
{
    return SDL_TRUE;
}

AudioBootStrap VSFAUDIO_bootstrap = {
    "vsf", "SDL VSF audio driver", VSFAudio_Init, SDL_FALSE
};

#endif

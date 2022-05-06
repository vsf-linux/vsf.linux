#include "vsf.h"

#undef _WIN32
#undef _MSC_VER
#undef __SSE2__

#define SDL_TIMER_UNIX
#define SDL_FILESYSTEM_UNIX
#define SDL_AUDIO_DRIVER_VSF 1
#define SDL_VIDEO_DRIVER_VSF 1
#define SDL_THREAD_PTHREAD 1

#define SDL_HAPTIC_DISABLED 1
#define SDL_SENSOR_DISABLED 1
#define SDL_JOYSTICK_DISABLED 1
#define SDL_JOYSTICK_DISABLED 1

#define HAVE_NANOSLEEP 1
#define HAVE_CLOCK_GETTIME 1

#define HAVE_LIBC 1
#define HAVE_SYS_TYPES_H 1
#define STDC_HEADERS 1
#define HAVE_STDIO_H 1
#define HAVE_STRING_H 1
#define HAVE_CTYPE_H 1
#define HAVE_MATH_H 1
#define HAVE_MALLOC 1
#define HAVE_GETENV 1
#define HAVE_PUTENV 1
#define HAVE_SETENV 1
#define HAVE_MEMSET 1
#define HAVE_MEMCPY 1
#define HAVE_MEMMOVE 1
#define HAVE_MEMCMP 1
#define HAVE_STRLEN 1
#define HAVE_STRCHR 1
#define HAVE_STRRCHR 1
#define HAVE_STRSTR 1
#define HAVE_ITOA 1
#define HAVE_ATOI 1
#define HAVE_STRTOL 1
#define HAVE_STRTOUL 1
#define HAVE_STRTOD 1
#define HAVE_STRTOLL 1
#define HAVE_STRTOULL 1
#define HAVE_STRCMP 1
#define HAVE_STRNCMP 1
#define HAVE_STRCASECMP 1
#define HAVE_VSNPRINTF 1
#define HAVE_TAN 1
#define HAVE_TANF 1
#define HAVE_ATAN 1
#define HAVE_ATAN2 1
#define HAVE_COS 1
#define HAVE_COSF 1
#define HAVE_SIN 1
#define HAVE_SINF 1
#define HAVE_EXP 1
#define HAVE_EXPF 1
#define HAVE_FABS 1
#define HAVE_FABSF 1
#define HAVE_FLOOR 1
#define HAVE_FLOORF 1
#define HAVE_TRUNC 1
#define HAVE_TRUNCF 1
#define HAVE_FMOD 1
#define HAVE_FMODF 1
#define HAVE_LOG 1
#define HAVE_LOGF 1
#define HAVE_LOG10 1
#define HAVE_LOG10F 1
#define HAVE_POW 1
#define HAVE_POWF 1
#define HAVE_ROUND 1
#define HAVE_ROUNDF 1
#define HAVE_LROUND 1
#define HAVE_LROUNDF 1
#define HAVE_SCALBN 1
#define HAVE_SCALBNF 1
#define HAVE_SQRT 1
#define HAVE_SQRTF 1
#define HAVE_COPYSIGN 1

typedef struct vsf_sdl2_cfg_t {
    vk_disp_t *disp_dev;
#if VSF_USE_AUDIO == ENABLED
    vk_audio_dev_t *audio_dev;
#endif
} vsf_sdl2_cfg_t;
typedef struct SDL_VSF_platform {
    vsf_sdl2_cfg_t cfg;

    struct {
        vk_input_notifier_t notifier;
        vsf_eda_t *eda_pending;
        vsf_slist_queue_t queue;
    } event;

    struct {
        char *text;
    } clipboard;

    bool is_disp_inited;
} SDL_VSF_platform;
extern SDL_VSF_platform SDL_platform;
extern void vsf_sdl2_init(vsf_sdl2_cfg_t *cfg);

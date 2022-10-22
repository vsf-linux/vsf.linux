#define __VSF_SIMPLE_STREAM_CLASS_INHERIT__
#include "../../../../raw/src/SDL_internal.h"

#if SDL_AUDIO_DRIVER_VSF

#include "SDL_audio.h"
#include "../../../../raw/src/audio/SDL_sysaudio.h"

#if VSF_USE_AUDIO == ENABLED

#define VSF_AUDIO_MOD               "VSF_AUDIO"
#define _THIS                       SDL_AudioDevice *_this

#ifndef SDL_AUDIO_VSF_BUFNUM
#   define SDL_AUDIO_VSF_BUFNUM     2
#endif

typedef struct VSFAUDIO_block_stream_item_t {
    vsf_block_stream_size_t size;
    uint8_t buffer[0];
} VSFAUDIO_block_stream_item_t;

typedef struct VSFAUDIO_ctx_t {
    uint32_t size;
    vk_audio_stream_t *audio_stream;
    vsf_eda_t *eda_pending;
    __vsf_block_stream_t stream;
    vsf_fifo_base_t fifo;
} VSFAUDIO_ctx_t;

static void __sdl_audio_playback_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    VSFAUDIO_ctx_t *ctx = (VSFAUDIO_ctx_t *)param;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_OUT: {
            vsf_eda_t *eda_pending = NULL;
            vsf_protect_t orig = vsf_protect_int();
                if (    (ctx->eda_pending != NULL)
                    &&  (vsf_stream_get_wbuf(stream, NULL) >= ctx->size)) {
                    eda_pending = ctx->eda_pending;
                    ctx->eda_pending = NULL;
                }
            vsf_unprotect_int(orig);

            if (eda_pending != NULL) {
                vsf_eda_post_evt(eda_pending, VSF_EVT_USER);
            }
        }
        break;
    }
}

static void __sdl_audio_capture_evthandler(vsf_stream_t *stream, void *param, vsf_stream_evt_t evt)
{
    VSFAUDIO_ctx_t *ctx = (VSFAUDIO_ctx_t *)param;

    switch (evt) {
    case VSF_STREAM_ON_CONNECT:
    case VSF_STREAM_ON_IN: {
            vsf_eda_t *eda_pending = NULL;
            vsf_protect_t orig = vsf_protect_int();
                if (    (ctx->eda_pending != NULL)
                    &&  (vsf_stream_get_rbuf(stream, NULL) >= ctx->size)) {
                    eda_pending = ctx->eda_pending;
                    ctx->eda_pending = NULL;
                }
            vsf_unprotect_int(orig);

            if (eda_pending != NULL) {
                vsf_eda_post_evt(eda_pending, VSF_EVT_USER);
            }
        }
        break;
    }
}

static int
VSFAUDIO_OpenDevice(_THIS, const char *devname)
{
    vk_audio_dev_t *audio_dev = NULL == _this->handle ?
                SDL_platform.cfg.audio_dev : _this->handle;
    vk_audio_stream_t *audio_stream = NULL;
    vk_audio_format_t vsf_audio_format_desired = {
        .datatype.value     = VSF_AUDIO_DATA_TYPE_LEU16,
        .channel_num        = _this->spec.channels,
        .sample_rate        = _this->spec.freq,
    };
    vk_audio_format_t vsf_audio_format;
    int first_stream_idx = -1;

    if (_this->iscapture) {
        return SDL_SetError("%s: Audio Capture is not supported now", VSF_AUDIO_MOD);
    }

    for (uint_fast8_t i = 0; i < audio_dev->stream_num; i++) {
        if (0 == audio_dev->stream[i].dir_in1out0) {
            if (first_stream_idx < 0) {
                first_stream_idx = i;
            }

            vsf_audio_format = audio_dev->stream[i].format;
            if (    !vsf_audio_format.value
                ||  (vsf_audio_format.value == vsf_audio_format_desired.value)) {
                audio_stream = &audio_dev->stream[i];
                break;
            }
        }
    }
    if (NULL == audio_stream) {
        if (first_stream_idx < 0) {
            return SDL_SetError("%s: Unsupported audio format", VSF_AUDIO_MOD);
        }

        audio_stream = &audio_dev->stream[first_stream_idx];
        vsf_audio_format = audio_dev->stream[first_stream_idx].format;

        _this->spec.channels = vsf_audio_format.channel_num;
        _this->spec.format =    (VSF_AUDIO_DATA_TYPE_BITLEN(vsf_audio_format.datatype.value) << 0)
                            |   (VSF_AUDIO_DATA_TYPE_IS_FLOAT(vsf_audio_format.datatype.value) ? SDL_AUDIO_MASK_DATATYPE : 0)
                            |   (VSF_AUDIO_DATA_TYPE_IS_BE(vsf_audio_format.datatype.value) ? SDL_AUDIO_MASK_ENDIAN : 0)
                            |   (VSF_AUDIO_DATA_TYPE_IS_SIGNED(vsf_audio_format.datatype.value) ? SDL_AUDIO_MASK_SIGNED : 0);
        _this->spec.freq = vsf_audio_format.sample_rate;
        SDL_CalculateAudioSpec(&_this->spec);
    } else if (0 == vsf_audio_format.value) {
        vsf_audio_format = vsf_audio_format_desired;
    }

    Uint32 block_size = (_this->spec.size + 3) & ~3;
    VSFAUDIO_ctx_t *ctx = SDL_calloc(1, sizeof(VSFAUDIO_ctx_t) +
        SDL_AUDIO_VSF_BUFNUM * (sizeof(VSFAUDIO_block_stream_item_t) + block_size));
    if (NULL == ctx) {
        return SDL_SetError("%s: Fail to allocate audio context", VSF_AUDIO_MOD);
    }
    ctx->size = _this->spec.size;
    ctx->audio_stream = audio_stream;
    ctx->stream.block_num = SDL_AUDIO_VSF_BUFNUM;
    ctx->stream.block_size = _this->spec.size;
    ctx->stream.op = &vsf_block_stream_op;
    VSF_STREAM_INIT(&ctx->stream);
    if (_this->iscapture) {
        ctx->stream.rx.param = ctx;
        ctx->stream.rx.evthandler = __sdl_audio_capture_evthandler;
        VSF_STREAM_CONNECT_RX(&ctx->stream);
    } else {
        ctx->stream.tx.param = ctx;
        ctx->stream.tx.evthandler = __sdl_audio_playback_evthandler;
        VSF_STREAM_CONNECT_TX(&ctx->stream);
    }

    vsf_err_t err = vk_audio_start(audio_dev, audio_stream->stream_index,
            &ctx->stream.use_as__vsf_stream_t, &vsf_audio_format);
    if (VSF_ERR_NONE != err) {
        SDL_free(ctx);
        return SDL_SetError("%s: Fail to start audio", VSF_AUDIO_MOD);
    }

    _this->hidden = (struct SDL_PrivateAudioData *)ctx;
    return 0;
}

static void
VSFAUDIO_PlayDevice(_THIS)
{
    VSFAUDIO_ctx_t *ctx = (VSFAUDIO_ctx_t *)_this->hidden;
    VSF_STREAM_WRITE(&ctx->stream, NULL, ctx->size);
}

static void
VSFAUDIO_WaitDevice(_THIS)
{
    VSFAUDIO_ctx_t *ctx = (VSFAUDIO_ctx_t *)_this->hidden;
    vsf_stream_t *stream = ctx->audio_stream->stream;
    SDL_bool is_capture = _this->iscapture;
    Uint32 size = ctx->size, stream_avail_size;
    vsf_eda_t *self = vsf_eda_get_cur();

    VSF_ASSERT(NULL == ctx->eda_pending);
    vsf_protect_t orig = vsf_protect_int();
        stream_avail_size = is_capture ?
                vsf_stream_get_rbuf(stream, NULL)
            :   vsf_stream_get_wbuf(stream, NULL);
        if (stream_avail_size < size) {
            ctx->eda_pending = self;
            self = NULL;
        }
    vsf_unprotect_int(orig);

    if (NULL == self) {
        vsf_thread_wfe(VSF_EVT_USER);
    }
}

static Uint8 *
VSFAUDIO_GetDeviceBuf(_THIS)
{
    VSFAUDIO_ctx_t *ctx = (VSFAUDIO_ctx_t *)_this->hidden;
    vsf_stream_t *stream = ctx->audio_stream->stream;
    Uint8 *buffer;
    uint_fast32_t size = _this->iscapture ?
            VSF_STREAM_GET_RBUF(stream, &buffer)
        :   VSF_STREAM_GET_WBUF(stream, &buffer);
    return (size < ctx->size) ? NULL : buffer;
}

static void
VSFAUDIO_CloseDevice(_THIS)
{
    vk_audio_dev_t *audio_dev = _this->handle;
    VSFAUDIO_ctx_t *ctx = (VSFAUDIO_ctx_t *)_this->hidden;
    vk_audio_stop(audio_dev, ctx->audio_stream->stream_index);
    SDL_free(_this->hidden);
}

static void
VSFAUDIO_DetectDevices(void)
{
    SDL_AddAudioDevice(SDL_FALSE, "vsf_audio", NULL, SDL_platform.cfg.audio_dev);
}
#endif

static SDL_bool
VSFAudio_Init(SDL_AudioDriverImpl * impl)
{
#if VSF_USE_AUDIO == ENABLED
    vk_audio_dev_t *audio_dev = SDL_platform.cfg.audio_dev;
    SDL_bool has_in = SDL_FALSE;

    if (!SDL_platform.is_audio_inited) {
        vk_audio_init(audio_dev);
        SDL_platform.is_audio_inited = true;
    }
    for (uint_fast8_t i = 0; i < audio_dev->stream_num; i++) {
        if (1 == audio_dev->stream[i].dir_in1out0) {
            has_in = SDL_TRUE;
        }
    }

    impl->DetectDevices = VSFAUDIO_DetectDevices;
    impl->OpenDevice = VSFAUDIO_OpenDevice;
    impl->PlayDevice = VSFAUDIO_PlayDevice;
    impl->WaitDevice = VSFAUDIO_WaitDevice;
    impl->GetDeviceBuf = VSFAUDIO_GetDeviceBuf;
    impl->CloseDevice = VSFAUDIO_CloseDevice;
    impl->HasCaptureSupport = has_in;
    return SDL_TRUE;
#else
    return SDL_FALSE;
#endif
}

AudioBootStrap VSFAUDIO_bootstrap = {
    "vsf", "SDL VSF audio driver", VSFAudio_Init, SDL_FALSE
};

#endif

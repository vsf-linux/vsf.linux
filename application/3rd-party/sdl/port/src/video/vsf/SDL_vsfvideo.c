#define __VSF_DISP_CLASS_INHERIT__
#define __VSF_EDA_CLASS_INHERIT__
#include "../../../../raw/src/SDL_internal.h"

#if SDL_VIDEO_DRIVER_VSF

#include "SDL_video.h"
#include "../../../../raw/src/video/SDL_sysvideo.h"
#include "../../../../raw/src/events/SDL_keyboard_c.h"
#include "../../../../raw/src/events/SDL_mouse_c.h"

#define VSF_VIDEO_MOD   "VSF_VIDEO"
#define VSF_SURFACE     "_SDL_VSFSurface"

typedef struct SDL_EventNode {
    vsf_slist_node_t node;
    vk_input_type_t type;
    vk_input_evt_t evt;
} SDL_EventNode;

static void
VSF_DeleteDevice(_THIS)
{
    SDL_free(_this);
}

static void
WIN_VideoInitReady(vk_disp_t *disp)
{
    vsf_eda_t *eda = disp->ui_data;
    VSF_ASSERT(eda != NULL);
    disp->ui_data = NULL;
    vsf_eda_post_evt(eda, VSF_EVT_RETURN);
}

static void
VSF_OnEvent(vk_input_notifier_t *notifier, vk_input_type_t type, vk_input_evt_t *evt)
{
    SDL_EventNode *node = vsf_heap_malloc(sizeof(SDL_EventNode));
    if (NULL == node) {
        VSF_ASSERT(false);
        return;
    }

    node->type = type;
    node->evt = *evt;
    vsf_slist_init_node(SDL_EventNode, node, node);
    vsf_protect_t orig = vsf_protect_int();
        vsf_slist_queue_enqueue(SDL_EventNode, node, &SDL_platform.event.queue, node);
        if (SDL_platform.event.eda_pending != NULL) {
            vsf_eda_post_evt(SDL_platform.event.eda_pending, VSF_EVT_USER);
        }
    vsf_unprotect_int(orig);
}

static void
VSF_PumpEvents(_THIS)
{
    SDL_EventNode *node = NULL;
    vsf_protect_t orig;
    vk_input_evt_t *evt;
    union {
        struct {
            uint16_t scancode;
            uint16_t keymod;
            SDL_Keycode keycode;
            char text[2];
            bool is_ext;
        } keyboard;
        struct {
            uint16_t x, y;
            uint8_t button;
        } mouse;
    } param;

    for (;;) {
        orig = vsf_protect_int();
        vsf_slist_queue_dequeue(SDL_EventNode, node, &SDL_platform.event.queue, node);
        vsf_unprotect_int(orig);

        if (NULL == node) {
            break;
        }

        evt = &node->evt;
        switch (node->type) {
        case VSF_INPUT_TYPE_KEYBOARD:
            param.keyboard.scancode = vsf_input_keyboard_get_keycode(evt);
            param.keyboard.is_ext = !!(param.keyboard.scancode & VSF_KB_EXT);
            param.keyboard.scancode &= ~VSF_KB_EXT;
            param.keyboard.keymod = vsf_input_keyboard_get_keymod(evt);
            param.keyboard.keycode = SDL_GetKeyFromScancode(param.keyboard.scancode);
            if (vsf_input_keyboard_is_down(evt)) {
                SDL_SendKeyboardKey(SDL_PRESSED, param.keyboard.scancode);
            } else {
                SDL_SendKeyboardKey(SDL_RELEASED, param.keyboard.scancode);
                if (    !(param.keyboard.keymod & (VSF_KM_CTRL | VSF_KM_ALT | VSF_KM_GUI))
                    &&  (param.keyboard.keycode >= ' ') && (param.keyboard.keycode <= '~')) {

                    if (param.keyboard.is_ext) {
                        if ((param.keyboard.keycode >= 'a') && (param.keyboard.keycode <= 'z')) {
                            param.keyboard.keycode -= 0x20;
                        } else {
                            switch (param.keyboard.keycode) {
                            case '`':   param.keyboard.keycode = '~';   break;
                            case '1':   param.keyboard.keycode = '!';   break;
                            case '2':   param.keyboard.keycode = '@';   break;
                            case '3':   param.keyboard.keycode = '#';   break;
                            case '4':   param.keyboard.keycode = '$';   break;
                            case '5':   param.keyboard.keycode = '%';   break;
                            case '6':   param.keyboard.keycode = '^';   break;
                            case '7':   param.keyboard.keycode = '&';   break;
                            case '8':   param.keyboard.keycode = '*';   break;
                            case '9':   param.keyboard.keycode = '(';   break;
                            case '0':   param.keyboard.keycode = ')';   break;
                            case '-':   param.keyboard.keycode = '_';   break;
                            case '=':   param.keyboard.keycode = '+';   break;
                            case '[':   param.keyboard.keycode = '{';   break;
                            case ']':   param.keyboard.keycode = '}';   break;
                            case '\\':  param.keyboard.keycode = '|';   break;
                            case ';':   param.keyboard.keycode = ':';   break;
                            case '\'':  param.keyboard.keycode = '\"';  break;
                            case ',':   param.keyboard.keycode = '<';   break;
                            case '.':   param.keyboard.keycode = '>';   break;
                            case '/':   param.keyboard.keycode = '?';   break;
                            }
                        }
                    }
                    param.keyboard.text[0] = param.keyboard.keycode;
                    param.keyboard.text[1] = '\0';
                    SDL_SendKeyboardText(param.keyboard.text);
                }
            }
            break;
        case VSF_INPUT_TYPE_MOUSE:
            param.mouse.x = vk_input_mouse_evt_get_x(evt);
            param.mouse.y = vk_input_mouse_evt_get_y(evt);
            switch (vk_input_mouse_evt_get(evt)) {
            case VSF_INPUT_MOUSE_EVT_MOVE:
                SDL_SendMouseMotion(NULL, 0, 0, param.mouse.x, param.mouse.y);
                break;
            case VSF_INPUT_MOUSE_EVT_BUTTON:
                param.mouse.button = vk_input_mouse_evt_button_get(evt);
                switch (param.mouse.button) {
                case VSF_INPUT_MOUSE_BUTTON_LEFT:
                    param.mouse.button = SDL_BUTTON_LEFT;
                    break;
                case VSF_INPUT_MOUSE_BUTTON_MIDDLE:
                    param.mouse.button = SDL_BUTTON_MIDDLE;
                    break;
                case VSF_INPUT_MOUSE_BUTTON_RIGHT:
                    param.mouse.button = SDL_BUTTON_RIGHT;
                    break;
                }
                if (vk_input_mouse_evt_button_is_down(evt)) {
                    SDL_SendMouseButton(NULL, 0, SDL_PRESSED, param.mouse.button);
                } else {
                    SDL_SendMouseButton(NULL, 0, SDL_RELEASED, param.mouse.button);
                }
                break;
            case VSF_INPUT_MOUSE_EVT_WHEEL:
                SDL_SendMouseWheel(NULL, 0, (int16_t)param.mouse.x, (int16_t)param.mouse.y, SDL_MOUSEWHEEL_NORMAL);
                break;
            }
            break;
        case VSF_INPUT_TYPE_TOUCHSCREEN:
            break;
        default:
            break;
        }

        vsf_heap_free(node);
    }
}

static int
VSF_WaitEventTimeout(_THIS, int timeout)
{
    vsf_eda_t *eda = vsf_eda_get_cur();
    bool is_empty;

    vsf_protect_t orig = vsf_protect_int();
        is_empty = vsf_slist_queue_is_empty(&SDL_platform.event.queue);
        if (is_empty && timeout > 0) {
            VSF_CAL_SECTION(".text.vsf.kernel.vsf_sync")
            vsf_eda_t * __vsf_eda_set_timeout(vsf_eda_t *eda, vsf_timeout_tick_t timeout);

            SDL_platform.event.eda_pending = vsf_eda_get_cur();
            __vsf_eda_set_timeout(eda, vsf_systimer_ms_to_tick(timeout));
        }
    vsf_unprotect_int(orig);

    if (is_empty && timeout > 0) {
        vsf_evt_t evt = vsf_thread_wait();
        if (evt == VSF_EVT_USER) {
            is_empty = false;
        }

        VSF_CAL_SECTION(".text.vsf.kernel.__vsf_teda_cancel_timer")
        vsf_err_t __vsf_teda_cancel_timer(vsf_teda_t *this_ptr);
        __vsf_teda_cancel_timer((vsf_teda_t *)eda);

        orig = vsf_protect_int();
            SDL_platform.event.eda_pending = NULL;
        vsf_unprotect_int(orig);
        eda->flag.state.is_limitted = false;
    }
    return is_empty ? 0 : 1;
}

static Uint32
VSF_GetSDLColorFormat(vk_disp_color_type_t vsf_disp_color)
{
    switch (vsf_disp_color) {
    case VSF_DISP_COLOR_ARGB8888:       return SDL_PIXELFORMAT_RGB888;
    case VSF_DISP_COLOR_RGB565:         return SDL_PIXELFORMAT_RGB565;
    default:    VSF_ASSERT(false);      return SDL_PIXELFORMAT_UNKNOWN;
    }
}

#if VSF_DISP_USE_FB == ENABLED
static SDL_bool
VSF_IsDisplayFB(vk_disp_t *disp)
{
    return SDL_FALSE;
//    return disp->param.drv == &vk_disp_drv_fb;
}
#endif

static void
VSF_DestroyWindowFramebuffer(_THIS, SDL_Window * window)
{
    SDL_Surface *surface;

    surface = (SDL_Surface *) SDL_SetWindowData(window, VSF_SURFACE, NULL);
    SDL_FreeSurface(surface);
}

static int
VSF_CreateWindowFramebuffer(_THIS, SDL_Window * window, Uint32 * format, void ** pixels, int *pitch)
{
    vk_disp_t *disp = (vk_disp_t *) _this->driverdata;
    SDL_Surface *surface;
    const Uint32 surface_format = VSF_GetSDLColorFormat(disp->param.color);
    int w, h;

    /* Free the old framebuffer surface */
    VSF_DestroyWindowFramebuffer(_this, window);

    /* Create a new one */
    SDL_GetWindowSize(window, &w, &h);

#if VSF_DISP_USE_FB == ENABLED
    if (VSF_IsDisplayFB(disp)) {
        surface = SDL_CreateRGBSurfaceWithFormatFrom(vk_disp_fb_get_back_buffer(disp), w, h, 0,
                            vsf_disp_get_pitch(disp), surface_format);
    } else
#endif
    {
        surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 0, surface_format);
    }
    if (!surface) {
        return -1;
    }

    /* Save the info and return! */
    SDL_SetWindowData(window, VSF_SURFACE, surface);
    *format = surface_format;
    *pixels = surface->pixels;
    *pitch = surface->pitch;
    return 0;
}

static int
VSF_UpdateWindowFramebuffer(_THIS, SDL_Window * window, const SDL_Rect * rects, int numrects)
{
    vk_disp_t *disp = (vk_disp_t *) _this->driverdata;
    SDL_Surface *surface;

    surface = (SDL_Surface *) SDL_GetWindowData(window, VSF_SURFACE);
    if (!surface) {
        return SDL_SetError("%s: Couldn't find dummy surface for window", VSF_VIDEO_MOD);
    }
    if ((surface->w > disp->param.width) || (surface->h > disp->param.height)) {
        return SDL_SetError("%s: Surface is too large to be fit in the screen", VSF_VIDEO_MOD);
    }

    disp->ui_data = vsf_eda_get_cur();
#if VSF_DISP_USE_FB == ENABLED
    if (VSF_IsDisplayFB(disp)) {
        vk_disp_fb_set_front_buffer(disp, -1);
        surface->pixels = vk_disp_fb_get_back_buffer(disp);
    } else
#endif
    {
        vk_disp_area_t area = {
            .pos.x      = 0,
            .pos.y      = 0,
            .size.x     = surface->w,
            .size.y     = surface->h,
        };
        vk_disp_refresh(disp, &area, surface->pixels);
    }
    vsf_thread_wfe(VSF_EVT_RETURN);
    return 0;
}

static int
VSF_VideoInit(_THIS)
{
    if (!SDL_platform.is_disp_inited) {
        vk_disp_t *disp = (vk_disp_t *) _this->driverdata;
        SDL_VideoDisplay display;
        SDL_DisplayMode mode = { 0 };

        disp->ui_data = vsf_eda_get_cur();
        disp->ui_on_ready = WIN_VideoInitReady;
        vk_disp_init(disp);
        vsf_thread_wfe(VSF_EVT_RETURN);

        mode.format = VSF_GetSDLColorFormat(disp->param.color);
        mode.h = disp->param.height;
        mode.w = disp->param.width;

        SDL_zero(display);
        display.name = "vsf_disp";
        display.desktop_mode = mode;
        display.current_mode = mode;
        display.orientation = SDL_ORIENTATION_UNKNOWN;
        if (SDL_AddDisplayMode(&display, &mode) < 0) {
            return -1;
        }
        if (SDL_AddVideoDisplay(&display, SDL_FALSE) < 0) {
            return -1;
        }
        _this->num_displays = 1;

        vsf_slist_queue_init(&SDL_platform.event.queue);
        SDL_platform.event.notifier.mask =  (1 << VSF_INPUT_TYPE_TOUCHSCREEN)
                                        |   (1 << VSF_INPUT_TYPE_KEYBOARD)
                                        |   (1 << VSF_INPUT_TYPE_MOUSE);
        SDL_platform.event.notifier.on_evt = (vk_input_on_evt_t)VSF_OnEvent;
        vk_input_notifier_register(&SDL_platform.event.notifier);

        SDL_platform.is_disp_inited = true;
    }
    return 0;
}

static void
VSF_VideoQuit(_THIS)
{
//    vk_disp_t *disp = (vk_disp_t *) _this->driverdata;
//    vk_disp_fini(disp);
}

static SDL_VideoDevice *
VSF_CreateDevice(int devindex)
{
    SDL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (SDL_VideoDevice *) SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (!device) {
        SDL_OutOfMemory();
        return (0);
    }
    device->driverdata = SDL_platform.cfg.disp_dev;

    /* Set the function pointers */
    device->VideoInit = VSF_VideoInit;
    device->VideoQuit = VSF_VideoQuit;
    device->PumpEvents = VSF_PumpEvents;
    device->WaitEventTimeout = VSF_WaitEventTimeout;
    device->CreateWindowFramebuffer = VSF_CreateWindowFramebuffer;
    device->UpdateWindowFramebuffer = VSF_UpdateWindowFramebuffer;
    device->DestroyWindowFramebuffer = VSF_DestroyWindowFramebuffer;

    device->free = VSF_DeleteDevice;

    return device;
}

VideoBootStrap VSF_bootstrap = {
    "vsf", "SDL VSF video driver", VSF_CreateDevice
};

#endif

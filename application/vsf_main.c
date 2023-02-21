#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_DISP_CLASS_INHERIT__
#include <unistd.h>

#if VSF_USE_USB_HOST == ENABLED && VSF_LINUX_USE_LIBUSB == ENABLED
#   include <libusb.h>
#endif

#include "usrapp_common.h"

#if VSF_USE_SDL2 == ENABLED
#   include <SDL2/SDL.h>
#endif

#ifndef VSF_APP_ENTRY
#   define VSF_APP_ENTRY        VSF_USER_ENTRY
#endif

#if APP_USE_LUA_DEMO == ENABLED && APP_USE_LUA_DEMO_LITE == ENABLED
static void __disp_on_inited(vk_disp_t *disp)
{
    vsf_eda_post_evt((vsf_eda_t *)disp->ui_data, VSF_EVT_USER);
}
#endif

#if APP_USE_SDLPAL_DEMO == ENABLED
#   include "3rd-party/sdl/sdlpal/font.h"
static int __sdlpal_main(int argc, char *argv[])
{
    // initialize font buffer first
#   ifdef __WIN__
    extern font_t *fontglyph_tw, __fontglyph_tw[];
    extern font_t *fontglyph_jp, __fontglyph_jp[];
    extern font_t *fontglyph_cn, __fontglyph_cn[];
    fontglyph_tw = &__fontglyph_tw[0];
    fontglyph_jp = &__fontglyph_jp[0];
    fontglyph_cn = &__fontglyph_cn[0];

    // unicode_font and font_width should be placed in ram with 65536 size
    extern unsigned char (*unicode_font)[32], __unicode_font[][32];
    extern unsigned char *font_width, __font_width[];
    unicode_font = &__unicode_font[0];
    font_width = &__font_width[0];
#   endif

    extern int sdlpal_main(int argc, char *argv[]);
    return sdlpal_main(argc, argv);
}
#endif

WEAK(vsf_board_init)
void vsf_board_init(void) {}

int vsf_linux_create_fhs(void)
{
    // 0. devfs, busybox, etc
    vsf_linux_vfs_init();
    busybox_install();

    // 1. hardware driver related demo
    vsf_board_init();
#if VSF_USE_USB_HOST == ENABLED
    usrapp_usbh_common_init();
#   if VSF_LINUX_USE_LIBUSB == ENABLED
    vsf_linux_libusb_startup();
#   endif
#endif

    // 2. fs
#if APP_USE_SCSI_DEMO == ENABLED
    mkdir("/scsi", 0);
#endif

    // 3. install executables
#if VSF_USE_LWIP == ENABLED && !defined(__AIC8800__)
    extern int lwip_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/lwip", lwip_main);
#endif
#if VSF_USE_USB_HOST == ENABLED && VSF_LINUX_USE_LIBUSB == ENABLED && APP_USE_LINUX_LIBUSB_DEMO == ENABLED
    extern int lsusb_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/lsusb", lsusb_main);
#   if APP_USE_LIBUVC_DEMO == ENABLED
    extern int uvc_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/uvc", uvc_main);
#   endif
#endif
#if APP_USE_LINUX_MOUNT_DEMO == ENABLED
    extern int mount_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/mount", mount_main);
#endif
#if APP_USE_LINUX_NTPDATE_DEMO == ENABLED
    extern int ntpdate_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/ntpdate", ntpdate_main);
#endif
#if APP_USE_LINUX_TELNETD_DEMO == ENABLED
    extern int telnetd_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/telnetd", telnetd_main);
#endif

#if APP_USE_LINUX_LESS_DEMO == ENABLED
    extern int less_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/less", less_main);
#endif
#if APP_USE_LINUX_CURL_DEMO == ENABLED
    extern int curl_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/curl", curl_main);
#endif
#if APP_USE_LINUX_GIT_DEMO == ENABLED
    extern int git_main(int argc, char **argv);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/git", git_main);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/git-remote-https", git_main);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/git-remote-http", git_main);
#endif
#if APP_USE_WAMR_DEMO == ENABLED
    extern int iwasm_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/iwasm", iwasm_main);
#endif
#if APP_USE_OPENOCD_DEMO == ENABLED
    extern int openocd_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/openocd", openocd_main);
#endif
#if APP_USE_XFEL_DEMO == ENABLED
    extern int xfel_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/xfel", xfel_main);
#endif

#if VSF_USE_SDL2 == ENABLED
    if (NULL == usrapp_ui_common.disp) {
        printf("waiting display...");
        while (NULL == usrapp_ui_common.disp) {
            sleep(1);
        }
    }
    *(vk_disp_color_type_t *)&usrapp_ui_common.disp->param.color = VSF_SDL2_CFG_COLOR;
    vsf_sdl2_cfg_t cfg = {
        .disp_dev = usrapp_ui_common.disp,
#if VSF_USE_AUDIO == ENABLED
        .audio_dev = usrapp_audio_common.default_dev,
#endif
    };
    vsf_sdl2_init(&cfg);

#if APP_USE_LUA_DEMO == ENABLED
    extern int lua_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/lua", lua_main);

#   if APP_USE_LUA_DEMO_LITE == ENABLED
    usrapp_ui_common.disp->ui_data = vsf_eda_get_cur();
    usrapp_ui_common.disp->ui_on_ready = __disp_on_inited;
    vk_disp_init(usrapp_ui_common.disp);
    vsf_thread_wfe(VSF_EVT_USER);

    extern int lite_main(int argc, char **argv);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/lite", lite_main);
#   endif
#endif

#if APP_USE_8086TINY_DEMO == ENABLED
    extern int x8086tiny_main(int argc, char **argv);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/8086tiny", x8086tiny_main);
#endif

#if APP_USE_SDLPAL_DEMO == ENABLED
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/sdlpal", __sdlpal_main);
#endif
#if APP_USE_DUNGEONRUSH_DEMO == ENABLED
    extern int dung_main(int argc, char **argv);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/dung", dung_main);
#endif
#if APP_USE_GNUBOY_DEMO == ENABLED
    extern int gnuboy_main(int argc, char **argv);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/gnuboy", gnuboy_main);
#endif
#if APP_USE_NOFRENDO_DEMO == ENABLED
    extern int nofrendo_main(int argc, char **argv);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/nofrendo", nofrendo_main);
#endif
#if APP_USE_MGBA_DEMO == ENABLED
    extern int mgba_main(int argc, char **argv);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/mgba", mgba_main);
#endif
#if APP_USE_BOCHS_DEMO == ENABLED
    extern int bochs_main(int argc, char **argv);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/bochs", bochs_main);
#endif
#endif      // VSF_USE_SDL2

#if VSF_USE_QUICKJS == ENABLED
#if APP_USE_MEUI_DEMO == ENABLED
    extern int meui_main(int argc, char **argv);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/meui", meui_main);
#endif
#endif

#if APP_USE_AUDIO_DEMO == ENABLED
    extern int audio_play_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/play_audio", audio_play_main);
#endif
#if APP_USE_QT_DEMO == ENABLED
    extern int qt_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/qt_demo", qt_main);
#endif

#if APP_USE_LWS_DEMO == ENABLED
    mkdir("/etc", 0);
    FILE *f = fopen("/etc/resolv.conf", "w+");
    if (f != NULL) {
        char *dns_cfg = "nameserver 192.168.1.1";
        fwrite(dns_cfg, strlen(dns_cfg), 1, f);
    }
    extern int lws_minimal_http_client_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/lws-minimal-http-client", lws_minimal_http_client_main);
#endif

#if VSF_USE_LOADER == ENABLED && VSF_LOADER_USE_ELF == ENABLED && APP_USE_LINUX_ELFLOADER_DEMO == ENABLED
    extern int elfloader_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/loadelf", elfloader_main);
#endif

#if APP_USE_USRAPP == ENABLED
    extern int usr_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/app", usr_main);
#endif
    return 0;
}

// TODO: SDL require that main need argc and argv
int VSF_APP_ENTRY(int argc, char *argv[])
{
    VSF_STREAM_INIT(&VSF_DEBUG_STREAM_RX);
    VSF_STREAM_INIT(&VSF_DEBUG_STREAM_TX);

#if VSF_USE_TRACE == ENABLED
    vsf_start_trace();
    vsf_stdio_init();
#endif

    vsf_trace_info("start linux..." VSF_TRACE_CFG_LINEEND);
    vsf_linux_stdio_stream_t stream = {
        .in     = (vsf_stream_t *)&VSF_DEBUG_STREAM_RX,
        .out    = (vsf_stream_t *)&VSF_DEBUG_STREAM_TX,
        .err    = (vsf_stream_t *)&VSF_DEBUG_STREAM_TX,
    };
    vsf_linux_init(&stream);
    return 0;
}

/* EOF */
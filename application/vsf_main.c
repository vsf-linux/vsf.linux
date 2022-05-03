#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_DISP_CLASS_INHERIT__
#include <unistd.h>
#include <libusb.h>

#include "usrapp_common.h"

#if APP_USE_LUA_DEMO == ENABLED && APP_USE_LUA_DEMO_LITE == ENABLED
static void __disp_on_inited(vk_disp_t *disp)
{
    vsf_eda_post_evt((vsf_eda_t *)disp->ui_data, VSF_EVT_USER);
}
#endif

int vsf_linux_create_fhs(void)
{
    // 0. devfs, busybox, etc
    vsf_linux_vfs_init();
    busybox_install();

    // 1. hardware driver related demo
#ifdef __AIC8800__
    extern void aic8800_demo_init(void);
    aic8800_demo_init();
#endif
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
#if APP_USE_LINUX_LIBUSB_DEMO == ENABLED
    extern int lsusb_main(int argc, char *argv[]);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/lsusb", lsusb_main);
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

#if VSF_USE_SDL2 == ENABLED
    *(vk_disp_color_type_t *)&usrapp_ui_common.disp->param.color = VSF_DISP_COLOR_ARGB8888;
    vsf_sdl2_init(usrapp_ui_common.disp);

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
    extern int _8086tiny_main(int argc, char **argv);
    busybox_bind(VSF_LINUX_CFG_BIN_PATH "/8086tiny", _8086tiny_main);
#endif

#endif      // VSF_USE_SDL2
    return 0;
}

// TODO: SDL require that main need argc and argv
int VSF_USER_ENTRY(void)
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
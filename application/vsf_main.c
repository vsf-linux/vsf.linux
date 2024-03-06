#define __VSF_EDA_CLASS_INHERIT__
#define __VSF_DISP_CLASS_INHERIT__
#include <unistd.h>

#if APP_USE_LINUX_XONE_DEMO == ENABLED
#   include <linux/workqueue.h>
#   include <linux/usb.h>
#endif
#if APP_USE_LINUX_BUSYBOX_DEMO == ENABLED
#   include <spawn.h>
#   include <sys/wait.h>
#endif

#if VSF_USE_USB_HOST == ENABLED && VSF_LINUX_USE_LIBUSB == ENABLED
#   include <libusb.h>
#endif

#include "usrapp_common.h"

#if VSF_USE_SDL2 == ENABLED
#   include <SDL2/SDL.h>
#endif

#include <fcntl.h>
#include <stdio.h>

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

#if APP_USE_LINUX_BUSYBOX_DEMO == ENABLED
extern int lbb_main(int argc, char *argv[]);
static int __busybox_export(int argc, char *argv[])
{
    char path[PATH_MAX], *app_pos;
    strcpy(path, VSF_LINUX_CFG_BIN_PATH "/");
    app_pos = &path[strlen(path)];

    argv++;
    argc--;
    while (argc-- > 0) {
        strcpy(app_pos, *argv++);
        vsf_linux_fs_bind_executable(path, lbb_main);
    }
    return 0;
}
#endif

VSF_CAL_WEAK(vsf_board_init)
void vsf_board_init(void) {}

int vsf_linux_create_fhs(void)
{
    // 0. devfs, busybox, etc
    vsf_linux_vfs_init();
#if VSF_LINUX_USE_BUSYBOX == ENABLED
    busybox_install();
#endif

    // 1. hardware driver related demo
    vsf_board_init();
#if VSF_USE_USB_HOST == ENABLED
    usrapp_usbh_common_init();
#   if VSF_LINUX_USE_LIBUSB == ENABLED
    vsf_linux_libusb_startup();
#   endif
#endif
#if VSF_USE_UI == ENABLED
    if (NULL == usrapp_ui_common.disp) {
        printf("waiting display...");
        while (NULL == usrapp_ui_common.disp) {
            sleep(1);
        }
    }
    vsf_linux_fs_bind_disp("/dev/fb0", usrapp_ui_common.disp);
#endif
#if VSF_USE_INPUT == ENABLED
    static vk_input_notifier_t __event_notifier_keyboard = {
        .mask = 1 << VSF_INPUT_TYPE_KEYBOARD,
    };
    vsf_linux_fs_bind_input("/dev/event/input0", &__event_notifier_keyboard);

    static vk_input_notifier_t __event_notifier_mouse = {
        .mask = 1 << VSF_INPUT_TYPE_MOUSE,
    };
    vsf_linux_fs_bind_input("/dev/event/input1", &__event_notifier_mouse);

    static vk_input_notifier_t __event_notifier_touchscreen = {
        .mask = 1 << VSF_INPUT_TYPE_TOUCHSCREEN,
    };
    vsf_linux_fs_bind_input("/dev/event/input2", &__event_notifier_touchscreen);
#endif
#if VSF_USE_SDL2 == ENABLED
    *(vk_disp_color_type_t *)&usrapp_ui_common.disp->param.color = VSF_SDL2_CFG_COLOR;
    vsf_sdl2_cfg_t cfg = {
        .disp_dev = usrapp_ui_common.disp,
#   if VSF_USE_AUDIO == ENABLED
        .audio_dev = usrapp_audio_common.default_dev,
#   endif
    };
    vsf_sdl2_init(&cfg);
#endif
#if APP_USE_LINUX_XONE_DEMO == ENABLED
    vsf_linux_usb_init(&usrapp_usbh_common.host);
    system_wq = create_workqueue("system_wq");
#endif

    // 2. fs
#if APP_USE_SCSI_DEMO == ENABLED
    mkdir("/scsi", 0);
#endif

    // 3. install executables
#if VSF_USE_LWIP == ENABLED && !defined(__AIC8800__)
    extern int lwip_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/lwip", lwip_main);
#endif
#if VSF_USE_USB_HOST == ENABLED && VSF_LINUX_USE_LIBUSB == ENABLED && APP_USE_LINUX_LIBUSB_DEMO == ENABLED
    extern int lsusb_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/lsusb", lsusb_main);
#   if APP_USE_LIBUVC_DEMO == ENABLED
    extern int uvc_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/uvc", uvc_main);
#   endif
#endif
#if APP_USE_LINUX_MOUNT_DEMO == ENABLED
    extern int mount_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/mount", mount_main);
#endif
#if APP_USE_LINUX_NTPDATE_DEMO == ENABLED
    extern int ntpdate_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/ntpdate", ntpdate_main);
#endif
#if APP_USE_LINUX_TELNETD_DEMO == ENABLED && VSF_LINUX_USE_BUSYBOX == ENABLED
    extern int telnetd_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/telnetd", telnetd_main);
#endif
#if APP_USE_LINUX_DYNLOADER_DEMO == ENABLED
    extern int dynloader_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/load", dynloader_main);
#endif

#if APP_USE_LINUX_LESS_DEMO == ENABLED
    extern int less_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/less", less_main);
#endif
#if APP_USE_LINUX_CURL_DEMO == ENABLED
    extern int curl_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/curl", curl_main);
#endif
#if APP_USE_LINUX_GIT_DEMO == ENABLED
    extern int git_main(int argc, char **argv);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/git", git_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/git-remote-https", git_main);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/git-remote-http", git_main);
#endif
#if APP_USE_WAMR_DEMO == ENABLED
    extern int iwasm_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/iwasm", iwasm_main);
#endif
#if APP_USE_OPENOCD_DEMO == ENABLED
    extern int openocd_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/openocd", openocd_main);
#endif
#if APP_USE_XFEL_DEMO == ENABLED
    extern int xfel_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/xfel", xfel_main);
#endif

#if VSF_USE_SDL2 == ENABLED
#if APP_USE_LUA_DEMO == ENABLED
    extern int lua_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/lua", lua_main);

#   if APP_USE_LUA_DEMO_LITE == ENABLED
    usrapp_ui_common.disp->ui_data = vsf_eda_get_cur();
    usrapp_ui_common.disp->ui_on_ready = __disp_on_inited;
    vk_disp_init(usrapp_ui_common.disp);
    vsf_thread_wfe(VSF_EVT_USER);

    extern int lite_main(int argc, char **argv);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/lite", lite_main);
#   endif
#endif

#if APP_USE_8086TINY_DEMO == ENABLED
    extern int x8086tiny_main(int argc, char **argv);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/8086tiny", x8086tiny_main);
#endif

#if APP_USE_SDLPAL_DEMO == ENABLED
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/sdlpal", __sdlpal_main);
#endif
#if APP_USE_DUNGEONRUSH_DEMO == ENABLED
    extern int dung_main(int argc, char **argv);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/dung", dung_main);
#endif
#if APP_USE_GNUBOY_DEMO == ENABLED
    extern int gnuboy_main(int argc, char **argv);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/gnuboy", gnuboy_main);
#endif
#if APP_USE_NOFRENDO_DEMO == ENABLED
    extern int nofrendo_main(int argc, char **argv);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/nofrendo", nofrendo_main);
#endif
#if APP_USE_MGBA_DEMO == ENABLED
    extern int mgba_main(int argc, char **argv);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/mgba", mgba_main);
#endif
#if APP_USE_BOCHS_DEMO == ENABLED
    extern int bochs_main(int argc, char **argv);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/bochs", bochs_main);
#endif
#if APP_USE_SDLVNC_DEMO == ENABLED
    extern int sdlvnc_main(int argc, char **argv);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/sdlvnc", sdlvnc_main);
#endif
#if APP_USE_FFMPEG_DEMO == ENABLED
    extern int ffplay_main(int argc, char **argv);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/ffplay", ffplay_main);
#endif
#endif      // VSF_USE_SDL2

#if VSF_USE_QUICKJS == ENABLED
#if APP_USE_MEUI_DEMO == ENABLED
    extern int meui_main(int argc, char **argv);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/meui", meui_main);
#endif
#endif

#if APP_USE_AUDIO_DEMO == ENABLED
    extern int audio_play_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/play_audio", audio_play_main);
#endif
#if APP_USE_QT_DEMO == ENABLED
    extern int qt_main(int argc, char *argv[]);
    putenv("QT_QPA_PLATFORM=linuxfb");
    putenv("QT_QPA_EVDEV_KEYBOARD_PARAMETERS=/dev/event/input0");
    putenv("QT_QPA_EVDEV_MOUSE_PARAMETERS=/dev/event/input1");
    putenv("QT_QPA_EVDEV_TOUCHSCREEN_PARAMETERS=/dev/event/input2");
    putenv("QT_QPA_FONTDIR=/mnt/hostfs/qt/lib/fonts");
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/qt_demo", qt_main);
#endif

#if APP_USE_LWS_DEMO == ENABLED
    mkdir("/etc", 0);
    FILE *f = fopen("/etc/resolv.conf", "w+");
    if (f != NULL) {
        char *dns_cfg = "nameserver 192.168.1.1";
        fwrite(dns_cfg, strlen(dns_cfg), 1, f);
    }
    extern int lws_minimal_http_client_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/lws-minimal-http-client", lws_minimal_http_client_main);
#endif

#if APP_USE_LINUX_TCC_DEMO == ENABLED
    extern int tcc_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/tcc", tcc_main);
#endif
#if APP_USE_LINUX_MAKE_DEMO == ENABLED
    extern int make_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/make", make_main);
#endif

#if APP_USE_LINUX_XONE_DEMO == ENABLED
    module_usb_driver_init(xone_dongle_driver);
    module_run_init(gip_bus_init);
#endif
#if APP_USE_USRAPP == ENABLED
    extern int usr_main(int argc, char *argv[]);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/app", usr_main);
#endif

#if APP_USE_LINUX_BUSYBOX_DEMO == ENABLED

#if defined(__WIN__) || defined(__LINUX__)
    int __vsf_linux_spawn(pid_t *pid, vsf_linux_main_entry_t entry,
                const posix_spawn_file_actions_t *actions,
                const posix_spawnattr_t *attr,
                char * const argv[], char * const env[], void *priv, int priv_size);

    // embedded busybox will not run, so setup basic directory structures here
    // temperary path to make mount command available
    putenv("PATH=/bin");
    pid_t pid;
    mkdir("/usr", 0);
    const char *mount_usr_argv[] = {
        "mount", "-t", VSF_LINUX_HOSTFS_TYPE, "./usr", "/usr", NULL,
    };
    if (!posix_spawnp(&pid, "mount", NULL, NULL, (char * const *)mount_usr_argv, NULL)) {
        waitpid(pid, NULL, 0);
    }

    mkdir("/etc", 0);
    const char *mount_etc_argv[] = {
        "mount", "-t", VSF_LINUX_HOSTFS_TYPE, "./etc", "/etc", NULL,
    };
    if (!posix_spawnp(&pid, "mount", NULL, NULL, (char * const *)mount_etc_argv, NULL)) {
        waitpid(pid, NULL, 0);
    }

    mkdir("/home", 0);
    const char *mount_home_argv[] = {
        "mount", "-t", VSF_LINUX_HOSTFS_TYPE, "./home", "/home", NULL,
    };
    if (!posix_spawnp(&pid, "mount", NULL, NULL, (char * const *)mount_home_argv, NULL)) {
        waitpid(pid, NULL, 0);
    }
#else
    const char *inittab_content = ":1:askfirst:/bin/sh\n";
    mkdir("/etc", 0);
    int fd = open("/etc/inittab", O_CREAT);
    if (fd >= 0) {
        write(fd, inittab_content, strlen(inittab_content));
        close(fd);
    }
#endif

    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/busybox_export", __busybox_export);
    vsf_linux_fs_bind_executable(VSF_LINUX_CFG_BIN_PATH "/busybox", lbb_main);
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/arch");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ascii");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/awk");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/base32");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/base64");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/basename");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/bc");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/bootchartd");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/bunzip2");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/cat");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/chat");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/chmod");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/cksum");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/clear");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/cmp");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/comm");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/cp");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/crc32");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/cttyhack");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/cut");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/date");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/dc");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/dd");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/devmem");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/df");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/diff");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/dirname");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/dos2unix");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/du");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/dumpkmap");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/echo");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ed");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/egrep");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/env");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/expand");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/expr");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/factor");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/false");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/fbsplash");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/fgconsole");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/fgrep");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/find");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/fold");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/free");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/fsync");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/grep");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/halt");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/head");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/hexedit");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/hostid");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/hush");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/i2cdetect");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/i2cdump");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/i2cget");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/i2cset");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/i2ctransfer");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/init");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/install");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/less");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/link");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/linuxrc");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ln");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/logname");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ls");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/lsscsi");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/man");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/md5sum");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/microcom");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/mim");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/mkdir");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/mkfifo");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/mknod");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/mktemp");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/mv");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/nc");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/nl");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/nohup");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/od");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/partprobe");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/paste");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/patch");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/poweroff");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/printenv");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/printf");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/pwd");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/readlink");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/realpath");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/reboot");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/reset");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/rm");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/rmdir");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/rx");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sed");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/seq");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/setkeycodes");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/setserial");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sh");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sha1sum");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sha256sum");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sha3sum");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sha512sum");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/showkey");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/shred");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/shuf");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sleep");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sort");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/split");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/stat");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/strings");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/stty");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sum");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/sync");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/tac");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/tail");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/tee");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/test");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/timeout");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/touch");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/tr");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/tree");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/true");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/truncate");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ts");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/tsort");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/tty");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ttysize");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/uname");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/unexpand");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/uniq");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/unix2dos");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/unlink");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/usleep");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/uudecode");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/uuencode");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/vi");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/volname");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/wc");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/which");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/whoami");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/xargs");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/yes");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ping");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/wget");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/telnetd");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/httpd");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/ssl_client");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/dpkg");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/gunzip");
    symlink(VSF_LINUX_CFG_BIN_PATH "/busybox", VSF_LINUX_CFG_BIN_PATH "/mkfs.vfat");
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
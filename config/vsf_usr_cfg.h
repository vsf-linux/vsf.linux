/*****************************************************************************
 *   Copyright(C)2009-2019 by VSF Team                                       *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 ****************************************************************************/


//! \note User Level Application Configuration

#ifndef __VSF_USR_CFG_H__
#define __VSF_USR_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#if     defined(__WIN__)
#   include "./target/vsf_usr_cfg_win.h"
#elif   defined(__LINUX__) || defined(__MACOS__)
#   include "./target/vsf_usr_cfg_linux.h"
#elif   defined(__AIC8800__)
#   include "./target/vsf_usr_cfg_aic8800.h"
#elif   defined(__STM32H743XI__)
#   include "./target/vsf_usr_cfg_stm32h743xi.h"
#endif

// components
#define VSF_USE_HASH                                    ENABLED
#   define VSF_HASH_USE_CRC                             ENABLED
#define VSF_USE_TRACE                                   ENABLED
#define VSF_USE_FIFO                                    ENABLED
#define VSF_USE_JSON                                    ENABLED
#define VSF_USE_DISTBUS                                 ENABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED
#define VSF_USE_STREAM                                  DISABLED
#define VSF_USE_LOADER                                  ENABLED
#   define VSF_LOADER_USE_ELF                           ENABLED
//#   define VSF_ELFLOADER_CFG_DEBUG                      ENABLED
#   ifdef __WIN__
#       define VSF_LOADER_USE_PE                        ENABLED
//#       define VSF_PELOADER_CFG_DEBUG                   ENABLED
#   endif

#if VSF_AUDIO_USE_WINSOUND == ENABLED || VSF_AUDIO_USE_DUMMY == ENABLED
#   define VSF_USE_AUDIO                                ENABLED
#   define VSF_AUDIO_USE_PLAYBACK                       ENABLED
#endif

#define VSF_USE_MAL                                     ENABLED
#   define VSF_MAL_USE_FILE_MAL                         ENABLED
#   define VSF_MAL_USE_SCSI_MAL                         ENABLED
#define VSF_USE_FS                                      ENABLED
#   define VSF_FS_USE_FATFS                             ENABLED
#define VSF_USE_INPUT                                   ENABLED
#define VSF_USE_SCSI                                    ENABLED
#   define VSF_SCSI_USE_MAL_SCSI                        ENABLED
#ifndef __CPU_WEBASSEMBLY__
#define VSF_USE_USB_HOST                                ENABLED
#endif
#   define VSF_USBH_USE_LIBUSB                          ENABLED
#   define VSF_USBH_USE_MSC                             ENABLED
#   define VSF_USBH_USE_HID                             ENABLED
#   define VSF_USBH_USE_DS4                             ENABLED
#   define VSF_USBH_USE_NSPRO                           ENABLED
#   define VSF_USBH_USE_XB360                           ENABLED
#   define VSF_USBH_USE_DL1X5                           ENABLED
#   define VSF_USBH_USE_UAC                             ENABLED

#define VSF_USE_TCPIP                                   ENABLED
//#define VSF_USE_UI                                      ENABLED
#   define VSF_DISP_USE_DL1X5                           ENABLED

#define VSF_USE_LINUX                                   ENABLED
#   define VSF_USE_POSIX                                ENABLED
#   if APP_USE_LINUX_BUSYBOX_DEMO == ENABLED
#       define VSF_LINUX_USE_BUSYBOX                    DISABLED
#   else
#       define VSF_LINUX_USE_BUSYBOX                    ENABLED
#   endif
#   define VSF_LINUX_CFG_LINK_FILE                      ENABLED
#   define VSF_LINUX_USE_SOCKET                         ENABLED
#       define VSF_LINUX_SOCKET_USE_UNIX                ENABLED
#       define VSF_LINUX_SOCKET_USE_INET                VSF_USE_TCPIP
#   define VSF_LINUX_USE_DEVFS                          ENABLED
#       define VSF_LINUX_DEVFS_USE_RAND                 ENABLED
#   define VSF_LINUX_CFG_FD_BITMAP_SIZE                 256
#   define VSF_LINUX_USE_LIBUSB                         VSF_USE_USB_HOST
#   define VSF_LINUX_CFG_PLS_NUM                        16
#   define VSF_LINUX_CFG_TLS_NUM                        64
#   if VSF_USE_LOADER == ENABLED
#       define VSF_USE_APPLET                           ENABLED
#       define VSF_LINUX_USE_APPLET                     ENABLED
#   endif

// APP configuration
#define APP_USE_LINUX_DEMO                              ENABLED
#define APP_USE_LINUX_MOUNT_DEMO                        ENABLED
#define APP_USE_LINUX_DYNLOADER_DEMO                    DISABLED
#if VSF_USE_USB_HOST == ENABLED && VSF_LINUX_USE_LIBUSB == ENABLED
#   define APP_USE_LINUX_LIBUSB_DEMO                    ENABLED
#endif
#if VSF_LINUX_USE_SOCKET == ENABLED && VSF_LINUX_SOCKET_USE_INET == ENABLED
#   define APP_USE_LINUX_NTPDATE_DEMO                   ENABLED
#   define APP_USE_LINUX_TELNETD_DEMO                   ENABLED
#endif
#define APP_USE_SCSI_DEMO                               ENABLED

// linux init scripts
#if     defined(__WIN__)
#   define VSF_LINUX_HOSTFS_TYPE                        "winfs"
#   define VSF_LINUX_PATH                               "../.."
#elif   defined(__LINUX__)
#   define VSF_LINUX_HOSTFS_TYPE                        "linfs"
#   define VSF_LINUX_PATH                               "../../.."
#else
#   define VSF_LINUX_HOSTFS_TYPE
#endif

#define VSF_LINUX_CFG_PATH                              "/bin:/usr.bin:/usr/local/bin:."
#define VSF_LINUX_CFG_INIT_SCRIPT_FILE                  "/home/root/.bashrc"
#define VSF_LINUX_HOSTFS_INIT_SCRIPTS                                           \
            "mkdir -p /mnt/hostfs",                                             \
            "mount -t " VSF_LINUX_HOSTFS_TYPE " . /mnt/hostfs",                 \
            "mkdir -p /usr",                                                    \
            "mount -t " VSF_LINUX_HOSTFS_TYPE " ./usr /usr",                    \
            "mkdir -p /home",                                                   \
            "mount -t " VSF_LINUX_HOSTFS_TYPE " ./home /home",                  \
            "mkdir -p /etc",                                                    \
            "mount -t " VSF_LINUX_HOSTFS_TYPE " ./etc /etc",                    \
            "export TERM=vt100",                                                \
            "export TERMINFO=/usr/share/terminfo",                              \
            "cd /home/root",

#if APP_USE_LINUX_GIT_DEMO == ENABLED
#   define VSF_LINUX_GIT_INIT_SCRIPTS                                           \
            "export GIT_SSL_NO_VERIFY=",
#else
#   define VSF_LINUX_GIT_INIT_SCRIPTS
#endif
#if APP_USE_LINUX_TCC_DEMO == ENABLED && defined(VSF_LINUX_PATH)
#   define VSF_LINUX_TCC_INIT_SCRIPTS                                           \
            "mkdir -p /mnt/vsf_linux",                                          \
            "mount -t " VSF_LINUX_HOSTFS_TYPE " " VSF_LINUX_PATH " /mnt/vsf_linux",\
            "mkdir -p /src/vsf",                                                \
            "mount -t " VSF_LINUX_HOSTFS_TYPE " " VSF_LINUX_PATH "/vsf /src/vsf",\
            "export VSF_LINUX_PATH=/mnt/vsf_linux",                             \
            "export VSF_PATH=/mnt/vsf_linux/vsf",                               \
            "export TCC_PATH=/mnt/vsf_linux/application/3rd-party/tcc/raw",     \
            "cd /mnt/vsf_linux/applet",
#else
#   define VSF_LINUX_TCC_INIT_SCRIPTS
#endif
#if APP_USE_USRAPP == ENABLED
#   define VSF_LINUX_USRAPP_INIT_SCRIPTS                                        \
            "app",
#else
#   define VSF_LINUX_USRAPP_INIT_SCRIPTS
#endif

#define VSF_LINUX_CFG_INIT_SCRIPTS                                              \
            "echo \"vsf build on " __DATE__ "\"",                               \
            VSF_LINUX_HOSTFS_INIT_SCRIPTS                                       \
            VSF_LINUX_GIT_INIT_SCRIPTS                                          \
            VSF_LINUX_USRAPP_INIT_SCRIPTS                                       \
            VSF_LINUX_TCC_INIT_SCRIPTS

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

#endif
/* EOF */

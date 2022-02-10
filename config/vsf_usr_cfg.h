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
#elif   defined(__LINUX__)
#   include "./target/vsf_usr_cfg_linux.h"
#endif

// components
#define VSF_USE_TRACE                                   ENABLED
#define VSF_USE_FIFO                                    ENABLED
#define VSF_USE_JSON                                    ENABLED
#define VSF_USE_DISTBUS                                 ENABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED
#define VSF_USE_STREAM                                  DISABLED

#define VSF_USE_MAL                                     ENABLED
#   define VSF_MAL_USE_FILE_MAL                         ENABLED
#define VSF_USE_FS                                      ENABLED
#define VSF_USE_INPUT                                   ENABLED
#define VSF_USE_SCSI                                    ENABLED
#define VSF_USE_USB_HOST                                ENABLED
//#   define VSF_USBH_USE_ECM                             ENABLED
#   define VSF_USBH_USE_LIBUSB                          ENABLED

#define VSF_USE_TCPIP                                   ENABLED
#   define VSF_USE_LWIP                                 ENABLED

#define VSF_USE_LINUX                                   ENABLED
#   define VSF_USE_POSIX                                ENABLED
#   define VSF_LINUX_USE_BUSYBOX                        ENABLED
#   define VSF_LINUX_USE_SOCKET                         ENABLED
#       define VSF_LINUX_SOCKET_USE_UNIX                ENABLED
#       define VSF_LINUX_SOCKET_USE_INET                ENABLED
#   define VSF_LINUX_USE_DEVFS                          ENABLED
#       define VSF_LINUX_DEVFS_USE_RAND                 ENABLED
#   define VSF_LINUX_CFG_FD_BITMAP_SIZE                 256
#   define VSF_LINUX_USE_LIBUSB                         VSF_USE_USB_HOST
#   define VSF_LINUX_CFG_PLS_NUM                        16
#   define VSF_LINUX_CFG_TLS_NUM                        64
#   define VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR     ENABLED
#       define VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_TRACE_DEPTH 8192
#       define VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_QUIET       ENABLED
#   define VSF_LINUX_CFG_RTC                            vsf_hw_rtc0

// APP configuration
#define APP_USE_LINUX_DEMO                              ENABLED
#define APP_USE_LINUX_MOUNT_DEMO                        ENABLED
#define APP_USE_LINUX_LIBUSB_DEMO                       ENABLED
#define APP_USE_LINUX_NTPDATE_DEMO                      ENABLED
#define APP_USE_LWIP_DEMO                               ENABLED

// for debug and test only
#define VSF_LINUX_CFG_INIT_SCRIPTS                                              \
            "lwip",                                                             \
            "ntpdate cn.pool.ntp.org",                                          \
            "mkdir -p /mnt/hostfs",                                             \
            "mount -t winfs z:\\test /mnt/hostfs",                              \
            "cd /mnt/hostfs",                                                   \
            "GIT_SSL_NO_VERIFY= git clone https://gitee.com/vsfos/vsf.git",

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ INCLUDES ======================================*/

#endif
/* EOF */

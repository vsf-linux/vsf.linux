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

#ifndef __VSF_USR_CFG_LINUX_H__
#define __VSF_USR_CFG_LINUX_H__

/*============================ INCLUDES ======================================*/

#define __VSF_HEADER_ONLY_SHOW_COMPILER_INFO__
#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/

// linux porting does not support preemption, so need evtq in idle
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     ENABLED

#   define VSF_LINUX_CFG_WRAPPER                        ENABLED
#   define VSF_LINUX_SOCKET_CFG_WRAPPER                 ENABLED
#   define VSF_LINUX_LIBUSB_CFG_WRAPPER                 ENABLED
#   define VSF_LINUX_USE_SIMPLE_LIBC                    ENABLED
#   define VSF_KERNEL_CFG_THREAD_STACK_LARGE            ENABLED
#   define VSF_LINUX_CFG_STACKSIZE                      (128 * 1024)
#   define VSF_LINUX_CFG_PRINT_BUFF_SIZE                4096
//#   define VSF_LINUX_CFG_RTC                            vsf_hw_rtc0
#   define VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR     ENABLED
#       define VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_TRACE_DEPTH (32 * 1024)
#       define VSF_LINUX_SIMPLE_STDLIB_HEAP_MONITOR_QUIET       ENABLED
#if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   define VSF_LINUX_LIBC_CFG_CPP                       ENABLED
// VSF_LINUX_LIBC_CFG_WRAPPER is required on __LINUX__ to avoid API confliction
#   define VSF_LINUX_LIBC_CFG_WRAPPER                   ENABLED
#endif

// component configure
#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_SIZE                                (256 * 1024 * 1024)
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT                   12      // 4K alignment

#define VSF_USE_TRACE                                   ENABLED
#define VSF_ASSERT(...)                                 assert(__VA_ARGS__)
//#define VSF_ASSERT(...)
#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED
#define VSF_SYSTIMER_FREQ                               (0ul)

// libusb does not support hub
#define VSF_USBH_CFG_ENABLE_ROOT_HUB                    DISABLED
#define VSF_USBH_USE_HUB                                DISABLED

#define VSF_FS_USE_LINFS                                ENABLED

#define VSF_DISP_USE_SDL2                               ENABLED
#   define VSF_DISP_SDL2_CFG_INCLUDE                    <SDL2/SDL.h>
#   define VSF_DISP_SDL2_CFG_MOUSE_AS_TOUCHSCREEN       ENABLED
#       define VSF_DISP_SDL2_CFG_HW_PRIORITY            vsf_arch_prio_0
#   define APP_DISP_SDL2_HEIGHT                         768
#   define APP_DISP_SDL2_WIDTH                          1024
#   define APP_DISP_SDL2_TITLE                          "vsf_screen"
#   define APP_DISP_SDL2_COLOR                          VSF_DISP_COLOR_RGB565
#   define APP_DISP_SDL2_AMPLIFIER                      1

#ifndef __CPU_WEBASSEMBLY__
#   define VSF_TRACE_CFG_COLOR_EN                       ENABLED
// TODO: enable curl if ready for webassembly
#   define APP_USE_LINUX_CURL_DEMO                      ENABLED

#   define VSF_USBH_USE_HCD_LIBUSB                      ENABLED
#       define VSF_LIBUSB_HCD_CFG_DEV_NUM               1
#       define VSF_LIBUSB_HCD_DEV0_VID                  0x0A12      // CSR8510 bthci
#       define VSF_LIBUSB_HCD_DEV0_PID                  0x0001

#   if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#       define VSF_USE_SIMPLE_SSCANF                    ENABLED
#       define VSF_USE_SIMPLE_SPRINTF                   ENABLED
#   endif
#else
#   define VSH_HAS_COLOR                                0
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_LINUX_H__
/* EOF */

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

#ifndef __VSF_USR_CFG_WIN_H__
#define __VSF_USR_CFG_WIN_H__

/*============================ INCLUDES ======================================*/

#define __VSF_HEADER_ONLY_SHOW_COMPILER_INFO__
#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/

// enable the 4 configurations below to use simple implementation for x86
//  simple implementation will not use ThreadSuspend and ThreadResume and has better CPU usage
//  **** but preempt is not supported ****
//  priority configurations are dependent on MACROs below, so put them here(at top)
//  IMPORTANT: vsf_arch_sleep MUST be called in vsf_plug_in_on_kernel_idle
#define VSF_ARCH_PRI_NUM                                1
#define VSF_ARCH_SWI_NUM                                0
#define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                     ENABLED
#define __VSF_X86_WIN_SINGLE_PRIORITY

//#define __WIN7__

#ifdef __VSF_X86_WIN_SINGLE_PRIORITY
#   define VSF_DEBUG_STREAM_CFG_HW_PRIORITY             vsf_arch_prio_0
#endif

#if __IS_COMPILER_LLVM__
#   pragma clang diagnostic ignored "-Wignored-attributes"
#   pragma clang diagnostic ignored "-Winvalid-noreturn"
#   pragma clang diagnostic ignored "-Wmacro-redefined"
#   pragma clang diagnostic ignored "-Wimplicit-function-declaration"
#endif

#   define VSF_LINUX_CFG_WRAPPER                        ENABLED
#   define VSF_LINUX_SOCKET_CFG_WRAPPER                 ENABLED
#   define VSF_LINUX_USE_SIMPLE_LIBC                    ENABLED
#   define VSF_KERNEL_CFG_THREAD_STACK_LARGE            ENABLED
#   define VSF_LINUX_CFG_STACKSIZE                      (128 * 1024)
#   define VSF_LINUX_CFG_PRINT_BUFF_SIZE                4096
#   define VSF_LINUX_CFG_RTC                            vsf_hw_rtc0
// if VSF_LINUX_USE_SIMPLE_LIBC is enabled, need VSF_USE_SIMPLE_SSCANF and VSF_USE_SIMPLE_SPRINTF
#if VSF_LINUX_USE_SIMPLE_LIBC == ENABLED
#   define VSF_LINUX_LIBC_CFG_CPP                       ENABLED
// VSF_LINUX_LIBC_CFG_WRAPPER is required on __WIN__ to avoid API confliction
#   define VSF_LINUX_LIBC_CFG_WRAPPER                   ENABLED
#   define VSF_USE_SIMPLE_SSCANF                        ENABLED
#   define VSF_USE_SIMPLE_SPRINTF                       ENABLED
#else
#   define VSF_USE_SIMPLE_SSCANF                        DISABLED
#   define VSF_USE_SIMPLE_SPRINTF                       DISABLED
#endif

// component configure
#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_SIZE                                (256 * 1024 * 1024)
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT                   12      // 4K alignment

// using vsf_linux_socket_inet_winsock2 now
//#   define VSF_USE_LWIP                                 ENABLED
//#define VSF_NETDRV_USE_WPCAP                            ENABLED
//#   ifdef __VSF_X86_WIN_SINGLE_PRIORITY
//#       define VSF_NETDRV_WPCAP_CFG_HW_PRIORITY         vsf_arch_prio_0
//#   else
//#       define VSF_NETDRV_WPCAP_CFG_HW_PRIORITY         vsf_arch_prio_1
//#endif

#define VSF_USE_TRACE                                   ENABLED
#define VSF_ASSERT(...)                                 assert(__VA_ARGS__)
//#define VSF_ASSERT(...)
#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED
#define VSF_SYSTIMER_FREQ                               (0ul)

// winusb does not support hub
#define VSF_USBH_CFG_ENABLE_ROOT_HUB                    DISABLED
#define VSF_USBH_USE_HUB                                DISABLED
#ifdef __WIN7__
// winusb seems fail on win7
#   define VSF_USBH_USE_HCD_LIBUSB                      ENABLED
#       define VSF_LIBUSB_CFG_INSTALL_DRIVER            DISABLED
#else
#   define VSF_USBH_USE_HCD_WINUSB                      ENABLED
#       define VSF_WINUSB_CFG_INSTALL_DRIVER            DISABLED
#endif

#if VSF_USBH_USE_HCD_WINUSB == ENABLED
#   define VSF_WINUSB_HCD_CFG_DEV_NUM                   1
#   define VSF_WINUSB_HCD_DEV0_VID                      0x0A12      // CSR8510 bthci
#   define VSF_WINUSB_HCD_DEV0_PID                      0x0001
#endif
#if VSF_USBH_USE_HCD_LIBUSB == ENABLED
// for libusb
//#   define VSF_LIBUSB_CFG_INCLUDE                       "lib\libusb\libusb.h"
// for libusb-win32
#   define VSF_LIBUSB_CFG_INCLUDE                       "lib\libusb-win32\lusb0_usb.h"
#   define VSF_LIBUSB_HCD_CFG_DEV_NUM                   1
#   define VSF_LIBUSB_HCD_DEV0_VID                      0x0A12      // CSR8510 bthci
#   define VSF_LIBUSB_HCD_DEV0_PID                      0x0001
#endif

#define VSF_FS_USE_WINFS                                ENABLED

#define VSF_DISP_USE_WINGDI                             ENABLED
//#define VSF_DISP_USE_SDL2                               ENABLED
//#   define VSF_DISP_SDL2_CFG_INCLUDE                    "lib\SDL2\include\SDL.h"
//#   define VSF_DISP_SDL2_CFG_MOUSE_AS_TOUCHSCREEN       ENABLED
//#   ifdef __VSF_X86_WIN_SINGLE_PRIORITY
//#       define VSF_DISP_SDL2_CFG_HW_PRIORITY            vsf_arch_prio_0
//#   else
//#       define VSF_DISP_SDL2_CFG_HW_PRIORITY            vsf_arch_prio_1
//#   endif

#define VSF_USE_WINSOUND                                ENABLED
#   define VSF_WINSOUND_CFG_TRACE                       DISABLED

#ifdef __WIN7__
// console for win7 does not support color by default
#   define VSF_TRACE_CFG_COLOR_EN                       DISABLED
#   define VSH_HAS_COLOR                                0
#   define VSF_WINUSB_CFG_WIN7                          ENABLED
#else
#    define VSF_TRACE_CFG_COLOR_EN                      ENABLED
#endif

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_WIN_H__
/* EOF */

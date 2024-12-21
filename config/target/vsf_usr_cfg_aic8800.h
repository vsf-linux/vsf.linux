/*****************************************************************************
 *   Copyright(C)2009-2022 by VSF Team                                       *
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

#ifndef __VSF_USR_CFG_AIC8800_H__
#define __VSF_USR_CFG_AIC8800_H__

/*============================ INCLUDES ======================================*/

#define __VSF_HEADER_ONLY_SHOW_COMPILER_INFO__
#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/

#if __IS_COMPILER_IAR__
#   define VSF_KERNEL_CFG_THREAD_STACK_CHECK            ENABLED
#   define VSF_KERNEL_GET_STACK_FROM_JMPBUF(__JMPBUF)   ((*(__JMPBUF))[4] & 0xFFFFFFFF)
#elif __IS_COMPILER_GCC__ || __IS_COMPILER_LLVM__
// strtoxxx in newlib has dependency issues, implement in simple_stdlib
#   define VSF_LINUX_SIMPLE_STDLIB_USE_STRTOXX          ENABLED
#endif
#define VSF_ARCH_USE_THREAD_REG                         ENABLED

#define VSF_SYSTIMER_FREQ                               (240UL * 1000 * 1000)
// configure pool and heap to avoid heap allocating in interrupt
#define VSF_OS_CFG_EVTQ_POOL_SIZE                       128
#define VSF_OS_CFG_DEFAULT_TASK_FRAME_POOL_SIZE         32
#define VSF_POOL_CFG_FEED_ON_HEAP                       DISABLED
// enable VSF_EDA_QUEUE_CFG_REGION and VSF_EDA_QUEUE_CFG_SUPPORT_ISR
#define VSF_EDA_QUEUE_CFG_REGION                        ENABLED
#define VSF_EDA_QUEUE_CFG_SUPPORT_ISR                   ENABLED
#define __USE_LOCAL_STDIO__
#define VSF_USE_SIMPLE_SPRINTF                          ENABLED
#define VSF_USE_SIMPLE_SSCANF                           ENABLED

// Use unused interrupt as SWI
#define VSF_DEV_SWI_NUM                                 4
#define VSF_DEV_SWI_LIST                                WDT3_IRQn, WDT2_IRQn, WDT1_IRQn, I2CS_IRQn
#define SWI0_IRQHandler                                 WDT3_IRQHandler
#define SWI1_IRQHandler                                 WDT2_IRQHandler
#define SWI2_IRQHandler                                 WDT1_IRQHandler
#define SWI3_IRQHandler                                 I2CS_IRQHandler

#define VSF_HAL_USE_RNG                                 ENABLED
#define VSF_HAL_USE_GPIO                                ENABLED
#define VSF_HAL_USE_SPI                                 ENABLED
#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED

// AIC8800 demos
#define AIC8800_APP_USE_WIFI_DEMO                       ENABLED
#define AIC8800_APP_USE_BT_DEMO                         ENABLED
#   define APP_USE_BTSTACK_DEMO                         ENABLED
#       define VSF_USE_BTSTACK                          ENABLED

// component configure
#define VSF_USE_LWIP                                    ENABLED
#define VSF_USE_WIFI                                    ENABLED

#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT                   4
#   define VSF_HEAP_ADDR                                0x00100000
#   define VSF_HEAP_SIZE                                0x50000

#define APP_USE_LINUX_TTY_DEMO                          ENABLED
//lwip in opensource SDK does not support netlink
#define VSF_LINUX_SOCKET_USE_NETLINK                    DISABLED
#define VSF_LINUX_USE_SIMPLE_LIBC                       ENABLED
#   if __IS_COMPILER_IAR__
// IAR: to support CPP in linux-subsystem, define MACFOs below, and don't include the simple_libc path
//  Note that __VSF_WORKAROUND_IAR_CPP__ MUST also be defined to support CPP
#       define VSF_LINUX_LIBC_CFG_CPP                   ENABLED
#       define VSF_LINUX_USE_SIMPLE_STDIO               ENABLED
#       define VSF_LINUX_USE_SIMPLE_STRING              ENABLED
#       define VSF_LINUX_USE_SIMPLE_TIME                ENABLED
#       define VSF_LINUX_USE_SIMPLE_STDLIB              ENABLED
#       define VSF_LINUX_USE_SIMPLE_CTYPE               ENABLED
#   else
#       define VSF_LINUX_LIBC_CFG_CPP                   ENABLED
#       define VSF_LINUX_USE_SIMPLE_STDIO               ENABLED
#       define VSF_LINUX_USE_SIMPLE_STRING              ENABLED
#       define VSF_LINUX_USE_SIMPLE_TIME                ENABLED
#       define VSF_LINUX_USE_SIMPLE_STDLIB              ENABLED
#       define VSF_LINUX_USE_SIMPLE_CTYPE               ENABLED
#   endif
#define VSF_LINUX_CFG_STACKSIZE                         (8 * 1024)
#define VSF_TRACE_CFG_COLOR_EN                          ENABLED
#define VSH_HAS_COLOR                                   1

//#define VSF_ASSERT(...)
#define VSF_ASSERT(...)                                 if (!(__VA_ARGS__)) {while(1);}

// usbh memory MUST be in 0x001A0000 - 0x001C7FFF
#define vsf_usbh_malloc                                 __vsf_usbh_malloc
#define vsf_usbh_malloc_aligned                         __vsf_usbh_malloc_aligned
#define vsf_usbh_free                                   __vsf_usbh_free

#define VSF_USBH_USE_HCD_DWCOTG                         ENABLED
//  AIC8800 need this
#   define VSF_DWCOTG_HCD_WORKAROUND_ALIGN_BUFFER_SIZE  4096
//  VSF_DWCOTG_HCD_CFG_ENABLE_ROOT_HUB is by default disabled, no need root_hub support
#   define VSF_USBH_CFG_ENABLE_ROOT_HUB                 DISABLED
#   define VSF_USBH_USE_HUB                             ENABLED

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_AIC8800_H__
/* EOF */

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

#ifndef __VSF_USR_CFG_STM32H743_H__
#define __VSF_USR_CFG_STM32H743_H__

/*============================ INCLUDES ======================================*/

#define __VSF_HEADER_ONLY_SHOW_COMPILER_INFO__
#include "utilities/compiler/compiler.h"

/*============================ MACROS ========================================*/

#if __IS_COMPILER_IAR__
#   define VSF_KERNEL_CFG_THREAD_STACK_CHECK            ENABLED
#   define VSF_KERNEL_GET_STACK_FROM_JMPBUF(__JMPBUF)   ((*(__JMPBUF))[4] & 0xFFFFFFFF)
#elif __IS_COMPILER_GCC__
// strtoxxx in newlib has dependency issues, implement in simple_stdlib
#   define VSF_LINUX_SIMPLE_STDLIB_USE_STRTOXX          ENABLED
#endif

#define VSF_SYSTIMER_FREQ                               (400UL * 1000 * 1000)
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

#define VSF_USE_HEAP                                    ENABLED
#   define VSF_HEAP_CFG_MCB_MAGIC_EN                    ENABLED
#   define VSF_HEAP_CFG_MCB_ALIGN_BIT                   4
#   define VSF_HEAP_ADDR                                0x30000000
#   define VSF_HEAP_SIZE                                0x48000

#define VSF_LINUX_USE_SIMPLE_LIBC                       ENABLED
#   if __IS_COMPILER_IAR__
// IAR: to support CPP in linux-subsystem, define MACFOs below, and don't include the simple_libc path
//  Note that __VSF_WORKAROUND_IAR_CPP__ MUST also be defined to support CPP
#       define VSF_LINUX_LIBC_CFG_CPP                   ENABLED
#       define VSF_LINUX_USE_SIMPLE_STDIO               DISABLED
#       define VSF_LINUX_USE_SIMPLE_STRING              DISABLED
#       define VSF_LINUX_USE_SIMPLE_TIME                DISABLED
#       define VSF_LINUX_USE_SIMPLE_STDLIB              ENABLED
#       define VSF_LINUX_USE_SIMPLE_CTYPE               DISABLED
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

// debug stream
#define VSF_CFG_DEBUG_STREAM_TX_T                       vsf_stream_t
#define VSF_CFG_DEBUG_STREAM_RX_DECLARE                 declare_mem_stream(VSF_DEBUG_STREAM_RX)
#define VSF_CFG_DEBUG_STREAM_RX_T                       vsf_mem_stream_type(VSF_DEBUG_STREAM_RX)
//#define VSF_HAL_USE_DEBUG_STREAM                        ENABLED
//#define VSF_DEBUGGER_CFG_CONSOLE                        VSF_DEBUGGER_CFG_CONSOLE_SEGGER_RTT

// display frame buffer
#define VSF_DISP_USE_FB                                 ENABLED
#   define APP_DISP_FB_HEIGHT                           272
#   define APP_DISP_FB_WIDTH                            480
#   define APP_DISP_FB_COLOR                            VSF_DISP_COLOR_RGB565
#   define APP_DISP_FB_NUM                              2
#   define APP_DISP_FB_DRV                              stm32_ltdc_fb_drv
#   define APP_DISP_FB_PARAM                            NULL
#   define APP_DISP_FB_BUFFER                           0x24000000

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/


#endif      // __VSF_USR_CFG_STM32H743_H__
/* EOF */

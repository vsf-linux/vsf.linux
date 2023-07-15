//! \note User Level Application Configuration

#ifndef __VSF_USR_CFG_H__
#define __VSF_USR_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define __VSF_APPLET__
#define __VSF_APPLET_LIB__
#define __LINUX__

// ARCH

#if     defined(__CPU_ARM__)
// no program_start from CMSIS Core
#   define __PROGRAM_START                              _start
#elif   defined(__CPU_X64__)
#   define __VSF64__

#   define VSF_OS_CFG_ADD_EVTQ_TO_IDLE                  ENABLED
#   define VSF_KERNEL_CFG_CPU_USAGE                     DISABLED
#   define VSF_KERNEL_CFG_EDA_SUPPORT_TIMER             ENABLED
#else
#   error target not supported
#endif

// components

//#define VSF_USE_TRACE                                   ENABLED
#define VSF_USE_MAL                                     ENABLED
#define VSF_USE_FS                                      ENABLED
#define VSF_USE_SIMPLE_STREAM                           ENABLED
#define VSF_USE_USB_HOST                                ENABLED
#define VSF_USE_HEAP                                    ENABLED

// linux

#define VSF_USE_LINUX                                   ENABLED
#   define VSF_LINUX_LIBC_USE_ENVIRON                   ENABLED
#   define VSF_LINUX_USE_SIMPLE_LIBC                    ENABLED
#   define VSF_LINUX_USE_LIBUSB                         VSF_USE_USB_HOST
#   define VSF_LINUX_USE_SOCKET                         ENABLED
#   define VSF_LINUX_USE_APPLET                         ENABLED

// APP configuration

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */

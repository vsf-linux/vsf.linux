//! \note User Level Application Configuration

#ifndef __VSF_USR_CFG_H__
#define __VSF_USR_CFG_H__

/*============================ INCLUDES ======================================*/
/*============================ MACROS ========================================*/

#define __CPU_ARM__
#define __VSF_APPLET__
// no program_start from CMSIS Core
#define __PROGRAM_START                                 _start

// components
#define VSF_USE_SIMPLE_STREAM                           ENABLED

// linux
#define VSF_USE_LINUX                                   ENABLED
#   define VSF_LINUX_LIBC_USE_ENVIRON                   ENABLED
#   define VSF_LINUX_USE_SIMPLE_LIBC                    ENABLED
#   define VSF_LINUX_USE_APPLET                         ENABLED
#       define VSF_LINUX_APPLET_USE_LIBC                ENABLED
#       define VSF_LINUX_APPLET_USE_UNISTD              ENABLED
#       define VSF_LINUX_APPLET_USE_PTHREAD             ENABLED
#       define VSF_LINUX_APPLET_USE_SEMAPHORE           ENABLED
#       define VSF_LINUX_APPLET_USE_POLL                ENABLED
#       define VSF_LINUX_APPLET_USE_NETDB               ENABLED
#       define VSF_LINUX_APPLET_USE_LIBGEN              ENABLED
#       define VSF_LINUX_APPLET_USE_IFADDRS             ENABLED
#       define VSF_LINUX_APPLET_USE_LIBGETOPT           ENABLED
#       define VSF_LINUX_APPLET_USE_FCNTL               ENABLED
#       define VSF_LINUX_APPLET_USE_DIRENT              ENABLED
#       define VSF_LINUX_APPLET_USE_SYS_EPOLL           ENABLED
#       define VSF_LINUX_APPLET_USE_SYS_EVENTFD         ENABLED
#       define VSF_LINUX_APPLET_USE_SYS_RANDOM          ENABLED
#       define VSF_LINUX_APPLET_USE_SYS_SELECT          ENABLED
#       define VSF_LINUX_APPLET_USE_SYS_SHM             ENABLED
#       define VSF_LINUX_APPLET_USE_SYS_STAT            ENABLED
#       define VSF_LINUX_APPLET_USE_SYS_TIME            ENABLED
#       define VSF_LINUX_APPLET_USE_SYS_UTSNAME         ENABLED
#       define VSF_LINUX_APPLET_USE_SYS_WAIT            ENABLED
#       define VSF_LINUX_APPLET_USE_SYS_SOCKET          ENABLED

// APP configuration

/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/

#endif
/* EOF */

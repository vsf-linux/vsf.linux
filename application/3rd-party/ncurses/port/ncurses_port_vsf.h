#ifndef __NCURSES_PORT_VSF__
#define __NCURSES_PORT_VSF__

#include "shell/sys/linux/vsf_linux.h"
#if VSF_LINUX_SIMPLE_STDLIB_CFG_HEAP_MONITOR == ENABLED
// map APIs used in ncurses to put resources in ncurses_resources_process
#   undef malloc
#   undef calloc
#   undef free
#   undef realloc
#   undef strdup

extern vsf_linux_process_t ncurses_resources_process;

#   define malloc(__size)          __malloc_ex(&ncurses_resources_process, (__size))
#   define calloc(__n, __size)     __calloc_ex(&ncurses_resources_process, (__n), (__size))
#   define free(__ptr)             __free_ex(&ncurses_resources_process, (__ptr))
#   define realloc(__ptr, __size)  __realloc_ex(&ncurses_resources_process, (__ptr), (__size))
#   define strdup(__str)           __strdup_ex(&ncurses_resources_process, (__str))
#endif

#endif      // __NCURSES_PORT_VSF__

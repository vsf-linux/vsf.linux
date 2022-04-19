/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#ifndef _PLATFORM_INTERNAL_H
#define _PLATFORM_INTERNAL_H

#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <malloc.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __WIN__
#   define BH_PLATFORM_WINDOWS      1
#endif

/* Default thread priority */
#define BH_THREAD_DEFAULT_PRIORITY  0

typedef pthread_t korp_thread;
typedef pthread_t korp_tid;
typedef pthread_mutex_t korp_mutex;
typedef pthread_cond_t korp_cond;

#define bh_socket_t int

#ifdef __WIN__
unsigned os_getpagesize();
void * os_mem_commit(void *ptr, size_t size, int flags);
void os_mem_decommit(void *ptr, size_t size);
#else
#   define os_getpagesize           getpagesize
#endif

#ifdef __cplusplus
}
#endif

#endif /* end of _PLATFORM_INTERNAL_H */

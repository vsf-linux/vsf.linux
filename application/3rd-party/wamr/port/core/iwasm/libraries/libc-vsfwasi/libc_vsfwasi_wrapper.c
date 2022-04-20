/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#define __VSF_LINUX_CLASS_INHERIT__
#define __VSF_LINUX_FS_CLASS_INHERIT__
#include "bh_platform.h"
#include "wasm_export.h"

#if VSF_LINUX_USE_SOCKET == ENABLED
#   include <sys/socket.h>
#   include "shell/sys/linux/kernel/socket/vsf_linux_socket.h"
#endif
#include <poll.h>
#include <sys/random.h>

#ifndef __cplusplus
#   define _Static_assert(...)
#endif
#include "../../../../raw/core/iwasm/libraries/libc-wasi/sandboxed-system-primitives/include/wasmtime_ssp.h"
#include "../../../../raw/core/iwasm/libraries/libc-wasi/sandboxed-system-primitives/src/rights.h"

/* clang-format off */
#define get_module_inst(exec_env) \
    wasm_runtime_get_module_inst(exec_env)

#define get_wasi_ctx(module_inst) \
    wasm_runtime_get_wasi_ctx(module_inst)

#define validate_app_addr(offset, size) \
    wasm_runtime_validate_app_addr(module_inst, offset, size)

#define validate_native_addr(addr, size) \
    wasm_runtime_validate_native_addr(module_inst, addr, size)

#define addr_app_to_native(offset) \
    wasm_runtime_addr_app_to_native(module_inst, offset)

#define addr_native_to_app(ptr) \
    wasm_runtime_addr_native_to_app(module_inst, ptr)

#define module_malloc(size, p_native_addr) \
    wasm_runtime_module_malloc(module_inst, size, p_native_addr)

#define module_free(offset) \
    wasm_runtime_module_free(module_inst, offset)
/* clang-format on */

typedef __wasi_size_t wasi_size_t;
typedef __wasi_errno_t wasi_errno_t;
typedef __wasi_fd_t wasi_fd_t;
typedef __wasi_clockid_t wasi_clockid_t;
typedef __wasi_timestamp_t wasi_timestamp_t;
typedef __wasi_filesize_t wasi_filesize_t;
typedef __wasi_prestat_t wasi_prestat_t;
typedef __wasi_filedelta_t wasi_filedelta_t;
typedef __wasi_whence_t wasi_whence_t;
typedef __wasi_fdflags_t wasi_fdflags_t;
typedef __wasi_rights_t wasi_rights_t;
typedef __wasi_advice_t wasi_advice_t;
typedef __wasi_lookupflags_t wasi_lookupflags_t;
typedef __wasi_preopentype_t wasi_preopentype_t;
typedef __wasi_fdstat_t wasi_fdstat_t;
typedef __wasi_oflags_t wasi_oflags_t;
typedef __wasi_dircookie_t wasi_dircookie_t;
typedef __wasi_filestat_t wasi_filestat_t;
typedef __wasi_fstflags_t wasi_fstflags_t;
typedef __wasi_subscription_t wasi_subscription_t;
typedef __wasi_event_t wasi_event_t;
typedef __wasi_exitcode_t wasi_exitcode_t;
typedef __wasi_signal_t wasi_signal_t;
typedef __wasi_riflags_t wasi_riflags_t;
typedef __wasi_roflags_t wasi_roflags_t;
typedef __wasi_siflags_t wasi_siflags_t;
typedef __wasi_sdflags_t wasi_sdflags_t;
typedef __wasi_iovec_t wasi_iovec_t;
typedef __wasi_ciovec_t wasi_ciovec_t;
typedef __wasi_address_family_t wasi_address_family_t;
typedef __wasi_sock_type_t wasi_sock_type_t;
typedef __wasi_ip_port_t wasi_ip_port_t;
typedef __wasi_addr_t wasi_addr_t;

typedef struct wasi_prestat_app {
    wasi_preopentype_t pr_type;
    uint32 pr_name_len;
} wasi_prestat_app_t;

typedef struct iovec_app {
    uint32 buf_offset;
    uint32 buf_len;
} iovec_app_t;

typedef struct wasm_ctx_t {
    int argc;
    char **argv;
} wasm_ctx_t;

extern vk_file_t * __vsf_linux_get_fs_ex(vsf_linux_process_t *process, int fd);

static int __wasi_ctx_pls = -1;

void * wasm_runtime_get_wasi_ctx(wasm_module_inst_t module_inst)
{
    if (__wasi_ctx_pls < 0) {
        return NULL;
    }

    vsf_linux_localstorage_t *pls = vsf_linux_pls_get(__wasi_ctx_pls);
    return pls->data;
}

static uint32 __wasi_args_sizes_get(wasm_ctx_t *wasm_ctx)
{
    uint32 argv_buf_size = 0;
    for (int i = 0; i < wasm_ctx->argc; i++) {
        argv_buf_size += strlen(wasm_ctx->argv[i]) + 1;
    }
    return argv_buf_size;
}

static wasi_errno_t
wasi_args_sizes_get(wasm_exec_env_t exec_env, uint32 *argc_app,
                    uint32 *argv_buf_size_app)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    if (!validate_native_addr(argc_app, sizeof(uint32))
        || !validate_native_addr(argv_buf_size_app, sizeof(uint32)))
        return (wasi_errno_t)-1;

    wasm_ctx_t *wasm_ctx = get_wasi_ctx(module_inst);
    *argc_app = (uint32)wasm_ctx->argc;
    *argv_buf_size_app = __wasi_args_sizes_get(wasm_ctx);
    return 0;
}

static wasi_errno_t
wasi_args_get(wasm_exec_env_t exec_env, uint32 *argv_offsets, char *argv_buf)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    wasm_ctx_t *wasm_ctx = get_wasi_ctx(module_inst);
    uint32 argv_buf_size = __wasi_args_sizes_get(wasm_ctx);

    uint64 total_size = sizeof(int32) * ((uint64)wasm_ctx->argc + 1);
    if (total_size >= UINT32_MAX
        || !validate_native_addr(argv_offsets, (uint32)total_size)
        || argv_buf_size >= UINT32_MAX
        || !validate_native_addr(argv_buf, (uint32)argv_buf_size))
        return (wasi_errno_t)-1;

    size_t len;
    for (uint32 i = 0; i < wasm_ctx->argc; i++) {
        len = strlen(wasm_ctx->argv[i]) + 1;
        memcpy(argv_buf, wasm_ctx->argv[i], len);
        argv_offsets[i] = addr_native_to_app(argv_buf);
        argv_buf += len;
    }

    return 0;
}

static wasi_errno_t
wasi_clock_res_get(wasm_exec_env_t exec_env, wasi_clockid_t clock_id,
                   wasi_timestamp_t *resolution)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    if (!validate_native_addr(resolution, sizeof(wasi_timestamp_t)))
        return (wasi_errno_t)-1;

    return clock_getres(clock_id, resolution);
}

static wasi_errno_t
wasi_clock_time_get(wasm_exec_env_t exec_env, wasi_clockid_t clock_id,
                    wasi_timestamp_t precision, wasi_timestamp_t *time)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    if (!validate_native_addr(time, sizeof(wasi_timestamp_t)))
        return (wasi_errno_t)-1;

    struct timespec ts;
    int ret = clock_gettime(clock_id, &ts);
    if (!ret) {
        *time = (wasi_timestamp_t)ts.tv_nsec + (wasi_timestamp_t)ts.tv_sec * 1000000000;
    }
    return ret;
}

static void __wasi_environ_sizes_get(char **env, uint32 *environ_count_app,
                                     uint32 *environ_buf_size_app)
{
    uint32 environ_count = 0, environ_buf_size = 0;
    if (env != NULL) {
        while (*env != NULL) {
            environ_count++;
            environ_buf_size += strlen(*env) + 1;
            env++;
        }
    }
    *environ_count_app = (uint32)environ_count;
    *environ_buf_size_app = (uint32)environ_buf_size;
}

static wasi_errno_t
wasi_environ_sizes_get(wasm_exec_env_t exec_env, uint32 *environ_count_app,
                       uint32 *environ_buf_size_app)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    uint32 environ_count, environ_buf_size;
    if (!validate_native_addr(environ_count_app, sizeof(uint32))
        || !validate_native_addr(environ_buf_size_app, sizeof(uint32)))
        return (wasi_errno_t)-1;

    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    char **env = process->__environ;
    __wasi_environ_sizes_get(env, &environ_count, &environ_buf_size);

    *environ_count_app = (uint32)environ_count;
    *environ_buf_size_app = (uint32)environ_buf_size;
    return 0;
}

static wasi_errno_t
wasi_environ_get(wasm_exec_env_t exec_env, uint32 *environ_offsets,
                 char *environ_buf)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    uint32 environ_count, environ_buf_size;
    uint64 total_size;

    vsf_linux_process_t *process = vsf_linux_get_cur_process();
    char **env = process->__environ;
    __wasi_environ_sizes_get(env, &environ_count, &environ_buf_size);

    total_size = sizeof(int32) * ((uint64)environ_count + 1);
    if (total_size >= UINT32_MAX
        || !validate_native_addr(environ_offsets, (uint32)total_size)
        || environ_buf_size >= UINT32_MAX
        || !validate_native_addr(environ_buf, (uint32)environ_buf_size))
        return (wasi_errno_t)-1;

    size_t len;
    for (uint32 i = 0; i < environ_count; i++) {
        len = strlen(*env) + 1;
        memcpy(environ_buf, *env, len);
        environ_offsets[i] = addr_native_to_app(environ_buf);
        environ_buf += len;
    }

    return 0;
}

static wasi_errno_t
wasi_fd_prestat_get(wasm_exec_env_t exec_env, wasi_fd_t fd,
                    wasi_prestat_app_t *prestat_app)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    if (!validate_native_addr(prestat_app, sizeof(wasi_prestat_app_t)))
        return (wasi_errno_t)-1;

    vk_file_t *file = __vsf_linux_get_fs_ex(NULL, fd);
    if (NULL == file)
        return (wasi_errno_t)-1;

    if (file->attr & VSF_FILE_ATTR_DIRECTORY) {
        prestat_app->pr_type = __WASI_PREOPENTYPE_DIR;
    } else {
        return (wasi_errno_t)-1;
    }
    prestat_app->pr_name_len = (uint32)strlen(file->name) + 1;
    return 0;
}

static wasi_errno_t
wasi_fd_prestat_dir_name(wasm_exec_env_t exec_env, wasi_fd_t fd, char *path,
                         uint32 path_len)
{
    vk_file_t *file = __vsf_linux_get_fs_ex(NULL, fd);
    if (NULL == file)
        return (wasi_errno_t)-1;

    bh_memcpy_s(path, (uint32)path_len, file->name, (uint32)path_len);
    return 0;
}

static wasi_errno_t
wasi_fd_close(wasm_exec_env_t exec_env, wasi_fd_t fd)
{
    return close(fd);
}

static wasi_errno_t
wasi_fd_datasync(wasm_exec_env_t exec_env, wasi_fd_t fd)
{
    return fdatasync(fd);
}

static wasi_errno_t
wasi_fd_pread(wasm_exec_env_t exec_env, wasi_fd_t fd, iovec_app_t *iovec_app,
              uint32 iovs_len, wasi_filesize_t offset, uint32 *nread_app)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    struct iovec iovec[iovs_len];
    uint64 total_size = sizeof(iovec_app_t) * (uint64)iovs_len;
    if (!validate_native_addr(nread_app, (uint32)sizeof(uint32))
        || total_size >= UINT32_MAX
        || !validate_native_addr(iovec_app, (uint32)total_size))
        return (wasi_errno_t)-1;

    for (uint32 i = 0; i < iovs_len; i++, iovec_app++) {
        if (!validate_app_addr(iovec_app->buf_offset, iovec_app->buf_len))
            return (wasi_errno_t)-1;
        iovec[i].iov_base = (void *)addr_app_to_native(iovec_app->buf_offset);
        iovec[i].iov_len = iovec_app->buf_len;
    }

    ssize_t size = preadv(fd, iovec, iovs_len, offset);
    if (size < 0)
        return (wasi_errno_t)-1;

    *nread_app = (uint32)size;
    return 0;
}

static wasi_errno_t
wasi_fd_pwrite(wasm_exec_env_t exec_env, wasi_fd_t fd,
               const iovec_app_t *iovec_app, uint32 iovs_len,
               wasi_filesize_t offset, uint32 *nwritten_app)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    struct iovec ciovec[iovs_len];
    uint64 total_size = sizeof(iovec_app_t) * (uint64)iovs_len;
    if (!validate_native_addr(nwritten_app, (uint32)sizeof(uint32))
        || total_size >= UINT32_MAX
        || !validate_native_addr((void *)iovec_app, (uint32)total_size))
        return (wasi_errno_t)-1;

    for (uint32 i = 0; i < iovs_len; i++, iovec_app++) {
        if (!validate_app_addr(iovec_app->buf_offset, iovec_app->buf_len))
            return (wasi_errno_t)-1;
        ciovec[i].iov_base = (char *)addr_app_to_native(iovec_app->buf_offset);
        ciovec[i].iov_len = iovec_app->buf_len;
    }

    ssize_t size = pwritev(fd, ciovec, iovs_len, offset);
    if (size < 0)
        return (wasi_errno_t)-1;

    *nwritten_app = (uint32)size;
    return 0;
}

static wasi_errno_t
wasi_fd_read(wasm_exec_env_t exec_env, wasi_fd_t fd,
             const iovec_app_t *iovec_app, uint32 iovs_len, uint32 *nread_app)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    struct iovec iovec[iovs_len];
    uint64 total_size = sizeof(iovec_app_t) * (uint64)iovs_len;
    if (!validate_native_addr(nread_app, (uint32)sizeof(uint32))
        || total_size >= UINT32_MAX
        || !validate_native_addr((void *)iovec_app, (uint32)total_size))
        return (wasi_errno_t)-1;

    for (uint32 i = 0; i < iovs_len; i++, iovec_app++) {
        if (!validate_app_addr(iovec_app->buf_offset, iovec_app->buf_len))
            return (wasi_errno_t)-1;
        iovec[i].iov_base = (void *)addr_app_to_native(iovec_app->buf_offset);
        iovec[i].iov_len = iovec_app->buf_len;
    }

    ssize_t size = readv(fd, iovec, iovs_len);
    if (size < 0)
        return (wasi_errno_t)-1;

    *nread_app = (uint32)size;
    return 0;
}

static wasi_errno_t
wasi_fd_write(wasm_exec_env_t exec_env, wasi_fd_t fd,
              const iovec_app_t *iovec_app, uint32 iovs_len,
              uint32 *nwritten_app)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    struct iovec ciovec[iovs_len];
    uint64 total_size = sizeof(iovec_app_t) * (uint64)iovs_len;
    if (!validate_native_addr(nwritten_app, (uint32)sizeof(uint32))
        || total_size >= UINT32_MAX
        || !validate_native_addr((void *)iovec_app, (uint32)total_size))
        return (wasi_errno_t)-1;

    for (uint32 i = 0; i < iovs_len; i++, iovec_app++) {
        if (!validate_app_addr(iovec_app->buf_offset, iovec_app->buf_len))
            return (wasi_errno_t)-1;
        ciovec[i].iov_base = (void *)addr_app_to_native(iovec_app->buf_offset);
        ciovec[i].iov_len = iovec_app->buf_len;
    }

    ssize_t size = writev(fd, ciovec, iovs_len);
    if (size < 0)
        return (wasi_errno_t)-1;

    *nwritten_app = (uint32)size;
    return 0;
}

static wasi_errno_t
wasi_fd_renumber(wasm_exec_env_t exec_env, wasi_fd_t from, wasi_fd_t to)
{
    return dup2(from, to);
}

static wasi_errno_t
wasi_fd_seek(wasm_exec_env_t exec_env, wasi_fd_t fd, wasi_filedelta_t offset,
             wasi_whence_t whence, wasi_filesize_t *newoffset)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    if (!validate_native_addr(newoffset, sizeof(wasi_filesize_t)))
        return (wasi_errno_t)-1;

    wasi_filesize_t new_offset = lseek(fd, offset, whence);
    *newoffset = new_offset;
    return new_offset < 0 ? (wasi_errno_t)-1 : 0;
}

static wasi_errno_t
wasi_fd_tell(wasm_exec_env_t exec_env, wasi_fd_t fd, wasi_filesize_t *newoffset)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    if (!validate_native_addr(newoffset, sizeof(wasi_filesize_t)))
        return (wasi_errno_t)-1;

    wasi_filesize_t new_offset = lseek(fd, 0, SEEK_CUR);
    *newoffset = new_offset;
    return new_offset < 0 ? (wasi_errno_t)-1 : 0;
}

static __wasi_filetype_t __wasi_fs_filetype_from_stat(wasi_fd_t fd, struct stat *fdstat)
{
    if (S_ISDIR(fdstat->st_mode))
        return __WASI_FILETYPE_DIRECTORY;
    else if (S_ISREG(fdstat->st_mode))
        return __WASI_FILETYPE_REGULAR_FILE;
#if VSF_LINUX_USE_SOCKET == ENABLED
    else if (S_ISSOCK(fdstat->st_mode)) {
        vsf_linux_fd_t *sfd = vsf_linux_fd_get(fd);
        if (NULL == sfd)
            return (wasi_errno_t)-1;

        vsf_linux_socket_priv_t *priv = (struct vsf_linux_socket_priv_t *)sfd->priv;
        if ((priv->domain == AF_INET) || (priv->domain == AF_INET6)) {
            if (priv->type == SOCK_STREAM)
                return __WASI_FILETYPE_SOCKET_STREAM;
            else if (priv->type == SOCK_DGRAM)
                return __WASI_FILETYPE_SOCKET_DGRAM;
            else
                return __WASI_FILETYPE_UNKNOWN;
        } else
            return __WASI_FILETYPE_UNKNOWN;
#endif
    } else
        return __WASI_FILETYPE_UNKNOWN;
}

static void __wasi_filestat_from_stat(wasi_fd_t fd, __wasi_filestat_t *filestat, struct stat *fdstat)
{
    filestat->st_dev = fdstat->st_dev;
    filestat->st_ino = fdstat->st_ino;
    filestat->st_nlink = fdstat->st_nlink;
    filestat->st_size = fdstat->st_size;
    filestat->st_atim = 0;
    filestat->st_mtim = 0;
    filestat->st_ctim = 0;
    filestat->st_filetype = __wasi_fs_filetype_from_stat(fd, fdstat);
}

static wasi_errno_t
wasi_fd_fdstat_get(wasm_exec_env_t exec_env, wasi_fd_t fd,
                   wasi_fdstat_t *fdstat_app)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    if (!validate_native_addr(fdstat_app, sizeof(wasi_fdstat_t)))
        return (wasi_errno_t)-1;

    struct stat fdstat;
    if (fstat(fd, &fdstat) < 0)
        return (wasi_errno_t)-1;

    fdstat_app->fs_filetype = __wasi_fs_filetype_from_stat(fd, &fdstat);

    fdstat_app->fs_flags = 0;
    int flags = fcntl(fd, F_GETFL);
    if (flags & O_APPEND)
        fdstat_app->fs_flags |= __WASI_FDFLAG_APPEND;
#ifdef O_DSYNC
    if (flags & O_DSYNC)
        fdstat_app->fs_flags |= __WASI_FDFLAG_DSYNC;
#endif
    if (flags & O_NONBLOCK)
        fdstat_app->fs_flags |= __WASI_FDFLAG_NONBLOCK;
#ifdef O_RSYNC
    if (flags & O_RSYNC)
        fdstat_app->fs_flags |= __WASI_FDFLAG_RSYNC;
#endif
#ifdef O_SYNC
    if (flags & O_SYNC)
        fdstat_app->fs_flags |= __WASI_FDFLAG_SYNC;
#endif

    fdstat_app->fs_rights_base = RIGHTS_ALL;
    fdstat_app->fs_rights_inheriting = RIGHTS_ALL;

    return 0;
}

static wasi_errno_t
wasi_fd_fdstat_set_flags(wasm_exec_env_t exec_env, wasi_fd_t fd,
                         wasi_fdflags_t flags)
{
    int fdflags = 0;
    if (flags & __WASI_FDFLAG_APPEND) {
        lseek(fd, 0, SEEK_END);
    }
#if defined(O_DSYNC) || defined(O_SYNC)
    if (flags & __WASI_FDFLAG_DSYNC)
#   if      defined(O_DSYNC)
        fdflags |= O_DSYNC;
#   elif    defined(O_SYNC)
        fdflags |= O_SYNC;
#   endif
#endif
    if (flags & __WASI_FDFLAG_NONBLOCK)
        fdflags |= O_NONBLOCK;
#if defined(O_RSYNC) || defined(O_SYNC)
    if (flags & __WASI_FDFLAG_RSYNC)
#   if      defined(O_RSYNC)
        fdflags |= O_RSYNC;
#   elif    defined(O_SYNC)
        fdflags |= O_SYNC;
#   endif
#endif
#if     defined(O_SYNC)
    if (fdflags & __WASI_FDFLAG_SYNC)
        fdflags |= O_SYNC;
#endif

    if (fcntl(fd, F_SETFL, fdflags) < 0)
        return (wasi_errno_t)-1;
    return 0;
}

static wasi_errno_t
wasi_fd_fdstat_set_rights(wasm_exec_env_t exec_env, wasi_fd_t fd,
                          wasi_rights_t fs_rights_base,
                          wasi_rights_t fs_rights_inheriting)
{
    return 0;
}

static wasi_errno_t
wasi_fd_sync(wasm_exec_env_t exec_env, wasi_fd_t fd)
{
    return fsync(fd);
}

static wasi_errno_t
wasi_fd_advise(wasm_exec_env_t exec_env, wasi_fd_t fd, wasi_filesize_t offset,
               wasi_filesize_t len, wasi_advice_t advice)
{
    return 0;
}

static wasi_errno_t
wasi_fd_allocate(wasm_exec_env_t exec_env, wasi_fd_t fd, wasi_filesize_t offset,
                 wasi_filesize_t len)
{
    struct stat fdstat;
    int ret = fstat(fd, &fdstat);
    if (ret)
        return (wasi_errno_t)-1;

    if (fdstat.st_size < offset + len)
        ret = ftruncate(fd, offset + len);
    return ret;
}

static wasi_errno_t
wasi_path_create_directory(wasm_exec_env_t exec_env, wasi_fd_t fd,
                           const char *path, uint32 path_len)
{
    return mkdirat(fd, path, 0777);
}

static wasi_errno_t
wasi_path_link(wasm_exec_env_t exec_env, wasi_fd_t old_fd,
               wasi_lookupflags_t old_flags, const char *old_path,
               uint32 old_path_len, wasi_fd_t new_fd, const char *new_path,
               uint32 new_path_len)
{
    return (wasi_errno_t)-1;
}

static wasi_errno_t
wasi_path_open(wasm_exec_env_t exec_env, wasi_fd_t dirfd,
               wasi_lookupflags_t dirflags, const char *path, uint32 path_len,
               wasi_oflags_t oflags, wasi_rights_t fs_rights_base,
               wasi_rights_t fs_rights_inheriting, wasi_fdflags_t fs_flags,
               wasi_fd_t *fd_app)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    if (!validate_native_addr(fd_app, sizeof(wasi_fd_t)))
        return (wasi_errno_t)-1;

    bool is_to_read =
        (fs_rights_base & (__WASI_RIGHT_FD_READ | __WASI_RIGHT_FD_READDIR)) != 0;
    bool is_to_write =
        (fs_rights_base
         & (__WASI_RIGHT_FD_DATASYNC | __WASI_RIGHT_FD_WRITE
            | __WASI_RIGHT_FD_ALLOCATE | __WASI_RIGHT_FD_FILESTAT_SET_SIZE))
        != 0;
    int fdflags = is_to_write ? is_to_read ? O_RDWR : O_WRONLY : O_RDONLY;
    if (oflags & __WASI_O_CREAT)
        fdflags |= O_CREAT;
    if (oflags & __WASI_O_DIRECTORY)
        fdflags |= O_DIRECTORY;
    if (oflags & __WASI_O_EXCL)
        fdflags |= O_EXCL;
    if (oflags & __WASI_O_TRUNC)
        fdflags |= O_TRUNC;
    if (fs_flags & __WASI_FDFLAG_APPEND)
        fdflags |= O_APPEND;

    int fd = openat(dirfd, path, fdflags, 0666);
    if (fd < 0)
        return (wasi_errno_t)-1;

    *fd_app = (wasi_fd_t)fd;
    return 0;
}

static wasi_errno_t
wasi_fd_readdir(wasm_exec_env_t exec_env, wasi_fd_t fd, void *buf,
                uint32 buf_len, wasi_dircookie_t cookie, uint32 *bufused_app)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    if (!validate_native_addr(bufused_app, sizeof(uint32)))
        return (wasi_errno_t)-1;

    DIR *dp = fdopendir(fd);
    if (NULL == dp)
        return (wasi_errno_t)-1;

    if (cookie == __WASI_DIRCOOKIE_START)
        rewinddir(dp);
    else
        seekdir(dp, (long)cookie);

    struct dirent *de;
    __wasi_dirent_t cde;
    size_t namelen, curlen;
    uint32_t bufremain = buf_len;
    while (bufremain > 0) {
        de = readdir(dp);
        if (NULL == de)
            break;

        namelen = strlen(de->d_name);
        cde.d_next = telldir(dp);
        cde.d_ino = de->d_ino;
        cde.d_namlen = namelen;
        switch (de->d_type) {
        case DT_BLK:    cde.d_type = __WASI_FILETYPE_BLOCK_DEVICE;      break;
        case DT_CHR:    cde.d_type = __WASI_FILETYPE_CHARACTER_DEVICE;  break;
        case DT_DIR:    cde.d_type = __WASI_FILETYPE_DIRECTORY;         break;
        case DT_LNK:    cde.d_type = __WASI_FILETYPE_SYMBOLIC_LINK;     break;
        case DT_REG:    cde.d_type = __WASI_FILETYPE_REGULAR_FILE;      break;
        default:        cde.d_type = __WASI_FILETYPE_UNKNOWN;           break;
        }

        curlen = min(bufremain, sizeof(cde));
        if (curlen > 0) {
            memcpy(buf, &cde, curlen);
            bufremain -= curlen;

            curlen = min(bufremain, namelen);
            if (curlen > 0) 
                memcpy(buf, de->d_name, curlen);
            bufremain -= curlen;
        }
        if (!curlen)
            break;
    }

    *bufused_app = buf_len - bufremain;
    return 0;
}

static wasi_errno_t
wasi_path_readlink(wasm_exec_env_t exec_env, wasi_fd_t fd, const char *path,
                   uint32 path_len, char *buf, uint32 buf_len,
                   uint32 *bufused_app)
{
    return (wasi_errno_t)-1;
}

static wasi_errno_t
wasi_path_rename(wasm_exec_env_t exec_env, wasi_fd_t old_fd,
                 const char *old_path, uint32 old_path_len, wasi_fd_t new_fd,
                 const char *new_path, uint32 new_path_len)
{
    return renameat(old_fd, old_path, new_fd, new_path);
}

static wasi_errno_t
wasi_fd_filestat_get(wasm_exec_env_t exec_env, wasi_fd_t fd,
                     wasi_filestat_t *filestat)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    if (!validate_native_addr(filestat, sizeof(wasi_filestat_t)))
        return (wasi_errno_t)-1;

    struct stat fdstat;
    if (fstat(fd, &fdstat) < 0)
        return (wasi_errno_t)-1;

    __wasi_filestat_from_stat(fd, filestat, &fdstat);
    return 0;
}

static wasi_errno_t
wasi_fd_filestat_set_times(wasm_exec_env_t exec_env, wasi_fd_t fd,
                           wasi_timestamp_t st_atim, wasi_timestamp_t st_mtim,
                           wasi_fstflags_t fstflags)
{
    return (wasi_errno_t)-1;
}

static wasi_errno_t
wasi_fd_filestat_set_size(wasm_exec_env_t exec_env, wasi_fd_t fd,
                          wasi_filesize_t st_size)
{
    return ftruncate(fd, (off_t)st_size);
}

static wasi_errno_t
wasi_path_filestat_get(wasm_exec_env_t exec_env, wasi_fd_t fd,
                       wasi_lookupflags_t flags, const char *path,
                       uint32 path_len, wasi_filestat_t *filestat)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    if (!validate_native_addr(filestat, sizeof(wasi_filestat_t)))
        return (wasi_errno_t)-1;

    struct stat fdstat;
    if (fstatat(fd, path, &fdstat, 0) < 0)
        return (wasi_errno_t)-1;

    __wasi_filestat_from_stat(fd, filestat, &fdstat);
    return 0;
}

static wasi_errno_t
wasi_path_filestat_set_times(wasm_exec_env_t exec_env, wasi_fd_t fd,
                             wasi_lookupflags_t flags, const char *path,
                             uint32 path_len, wasi_timestamp_t st_atim,
                             wasi_timestamp_t st_mtim, wasi_fstflags_t fstflags)
{
    return (wasi_errno_t)-1;
}

static wasi_errno_t
wasi_path_symlink(wasm_exec_env_t exec_env, const char *old_path,
                  uint32 old_path_len, wasi_fd_t fd, const char *new_path,
                  uint32 new_path_len)
{
    return (wasi_errno_t)-1;
}

static wasi_errno_t
wasi_path_unlink_file(wasm_exec_env_t exec_env, wasi_fd_t fd, const char *path,
                      uint32 path_len)
{
    return unlinkat(fd, path, 0);
}

static wasi_errno_t
wasi_path_remove_directory(wasm_exec_env_t exec_env, wasi_fd_t fd,
                           const char *path, uint32 path_len)
{
    return unlinkat(fd, path, 0);
}

static wasi_errno_t
wasi_poll_oneoff(wasm_exec_env_t exec_env, const wasi_subscription_t *in,
                 wasi_event_t *out, uint32 nsubscriptions, uint32 *nevents_app)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    if (!validate_native_addr((void *)in, sizeof(wasi_subscription_t))
        || !validate_native_addr(out, sizeof(wasi_event_t))
        || !validate_native_addr(nevents_app, sizeof(uint32)))
        return (wasi_errno_t)-1;

    int timeout = -1;
    struct pollfd pfds[nsubscriptions];
    const __wasi_subscription_t *s;
    for (uint32 i = 0; i < nsubscriptions; i++) {
        s = &in[i];
        switch (s->u.type) {
        case __WASI_EVENTTYPE_FD_READ:
        case __WASI_EVENTTYPE_FD_WRITE:
            pfds[i].fd = s->u.u.fd_readwrite.fd;
            pfds[i].events = s->u.type == __WASI_EVENTTYPE_FD_READ ? POLLIN : POLLOUT;
            break;
        case __WASI_EVENTTYPE_CLOCK:
            timeout = (s->u.u.clock.timeout + (1000000 - 1)) / 1000000;
            pfds[i].fd = -1;
            break;
        }
    }

    int ret = poll(pfds, nsubscriptions, timeout);
    if (ret < 0)
        return (wasi_errno_t)-1;

    *nevents_app = 0;
    for (uint32 i = 0; i < nsubscriptions; i++) {
        if ((pfds[i].fd >= 0) && (pfds[i].revents != 0)) {
            if (pfds[i].revents & POLLIN) {
                out[(*nevents_app)++] = (__wasi_event_t){
                    .userdata = in[i].userdata,
                    .type = in[i].u.type,
                    .u.fd_readwrite.nbytes = 1,
                };
            }
        }
    }
    return 0;
}

static void
wasi_proc_exit(wasm_exec_env_t exec_env, wasi_exitcode_t rval)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    /* Here throwing exception is just to let wasm app exit,
       the upper layer should clear the exception and return
       as normal */
    wasm_runtime_set_exception(module_inst, "wasi proc exit");
}

static wasi_errno_t
wasi_proc_raise(wasm_exec_env_t exec_env, wasi_signal_t sig)
{
    wasm_module_inst_t module_inst = get_module_inst(exec_env);
    char buf[32];

    snprintf(buf, sizeof(buf), "%s%d", "wasi proc raise ", sig);
    wasm_runtime_set_exception(module_inst, buf);
    return 0;
}

static wasi_errno_t
wasi_random_get(wasm_exec_env_t exec_env, void *buf, uint32 buf_len)
{
    getrandom(buf, buf_len, 0);
    return 0;
}

static wasi_errno_t
wasi_sock_accept(wasm_exec_env_t exec_env, wasi_fd_t fd, wasi_fd_t *fd_new)
{
    int new_sock = accept(fd, NULL, NULL);
    if (new_sock < 0)
        return (wasi_errno_t)-1;

    *fd_new = (wasi_fd_t)new_sock;
    return 0;
}

static wasi_errno_t
wasi_sock_addr_local(wasm_exec_env_t exec_env, wasi_fd_t fd, uint8 *buf,
                     wasi_size_t buf_len)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_addr_remote(wasm_exec_env_t exec_env, wasi_fd_t fd, uint8 *buf,
                      wasi_size_t buf_len)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_addr_resolve(wasm_exec_env_t exec_env, wasi_fd_t fd, const char *host,
                       wasi_ip_port_t port, uint8 *buf, wasi_size_t size)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_bind(wasm_exec_env_t exec_env, wasi_fd_t fd, wasi_addr_t *addr)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_connect(wasm_exec_env_t exec_env, wasi_fd_t fd, wasi_addr_t *addr)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_get_recv_buf_size(wasm_exec_env_t exec_env, wasi_fd_t fd,
                            wasi_size_t *size)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_get_reuse_addr(wasm_exec_env_t exec_env, wasi_fd_t fd, uint8 *reuse)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_get_reuse_port(wasm_exec_env_t exec_env, wasi_fd_t fd, uint8 *reuse)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_get_send_buf_size(wasm_exec_env_t exec_env, wasi_fd_t fd,
                            wasi_size_t *size)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_listen(wasm_exec_env_t exec_env, wasi_fd_t fd, uint32 backlog)
{
    return listen(fd, backlog);
}

static wasi_errno_t
wasi_sock_close(wasm_exec_env_t exec_env, wasi_fd_t fd)
{
    return close(fd);
}

static wasi_errno_t
wasi_sock_open(wasm_exec_env_t exec_env, wasi_fd_t poolfd,
               wasi_address_family_t af, wasi_sock_type_t socktype,
               wasi_fd_t *sockfd)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_set_recv_buf_size(wasm_exec_env_t exec_env, wasi_fd_t fd,
                            wasi_size_t size)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_set_reuse_addr(wasm_exec_env_t exec_env, wasi_fd_t fd, uint8 reuse)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_set_reuse_port(wasm_exec_env_t exec_env, wasi_fd_t fd, uint8 reuse)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_set_send_buf_size(wasm_exec_env_t exec_env, wasi_fd_t fd,
                            wasi_size_t size)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_recv(wasm_exec_env_t exec_env, wasi_fd_t sock, iovec_app_t *ri_data,
               uint32 ri_data_len, wasi_riflags_t ri_flags,
               uint32 *ro_datalen_app, wasi_roflags_t *ro_flags)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_send(wasm_exec_env_t exec_env, wasi_fd_t sock,
               const iovec_app_t *si_data, uint32 si_data_len,
               wasi_siflags_t si_flags, uint32 *so_datalen_app)
{
    return __WASI_ENOSYS;
}

static wasi_errno_t
wasi_sock_shutdown(wasm_exec_env_t exec_env, wasi_fd_t sock, wasi_sdflags_t how)
{
    return shutdown(sock, how);
}

static wasi_errno_t
wasi_sched_yield(wasm_exec_env_t exec_env)
{
    return sched_yield();
}

/* clang-format off */
#define REG_NATIVE_FUNC(func_name, signature) \
    { #func_name, wasi_##func_name, signature, NULL }
/* clang-format on */

static NativeSymbol native_symbols_libc_wasi[] = {
    REG_NATIVE_FUNC(args_get, "(**)i"),
    REG_NATIVE_FUNC(args_sizes_get, "(**)i"),
    REG_NATIVE_FUNC(clock_res_get, "(i*)i"),
    REG_NATIVE_FUNC(clock_time_get, "(iI*)i"),
    REG_NATIVE_FUNC(environ_get, "(**)i"),
    REG_NATIVE_FUNC(environ_sizes_get, "(**)i"),
    REG_NATIVE_FUNC(fd_prestat_get, "(i*)i"),
    REG_NATIVE_FUNC(fd_prestat_dir_name, "(i*~)i"),
    REG_NATIVE_FUNC(fd_close, "(i)i"),
    REG_NATIVE_FUNC(fd_datasync, "(i)i"),
    REG_NATIVE_FUNC(fd_pread, "(i*iI*)i"),
    REG_NATIVE_FUNC(fd_pwrite, "(i*iI*)i"),
    REG_NATIVE_FUNC(fd_read, "(i*i*)i"),
    REG_NATIVE_FUNC(fd_write, "(i*i*)i"),
    REG_NATIVE_FUNC(fd_renumber, "(ii)i"),
    REG_NATIVE_FUNC(fd_seek, "(iIi*)i"),
    REG_NATIVE_FUNC(fd_tell, "(i*)i"),
    REG_NATIVE_FUNC(fd_fdstat_get, "(i*)i"),
    REG_NATIVE_FUNC(fd_fdstat_set_flags, "(ii)i"),
    REG_NATIVE_FUNC(fd_fdstat_set_rights, "(iII)i"),
    REG_NATIVE_FUNC(fd_sync, "(i)i"),
    REG_NATIVE_FUNC(fd_advise, "(iIIi)i"),
    REG_NATIVE_FUNC(fd_allocate, "(iII)i"),
    REG_NATIVE_FUNC(path_create_directory, "(i*~)i"),
    REG_NATIVE_FUNC(path_link, "(ii*~i*~)i"),
    REG_NATIVE_FUNC(path_open, "(ii*~iIIi*)i"),
    REG_NATIVE_FUNC(fd_readdir, "(i*~I*)i"),
    REG_NATIVE_FUNC(path_readlink, "(i*~*~*)i"),
    REG_NATIVE_FUNC(path_rename, "(i*~i*~)i"),
    REG_NATIVE_FUNC(fd_filestat_get, "(i*)i"),
    REG_NATIVE_FUNC(fd_filestat_set_times, "(iIIi)i"),
    REG_NATIVE_FUNC(fd_filestat_set_size, "(iI)i"),
    REG_NATIVE_FUNC(path_filestat_get, "(ii*~*)i"),
    REG_NATIVE_FUNC(path_filestat_set_times, "(ii*~IIi)i"),
    REG_NATIVE_FUNC(path_symlink, "(*~i*~)i"),
    REG_NATIVE_FUNC(path_unlink_file, "(i*~)i"),
    REG_NATIVE_FUNC(path_remove_directory, "(i*~)i"),
    REG_NATIVE_FUNC(poll_oneoff, "(**i*)i"),
    REG_NATIVE_FUNC(proc_exit, "(i)"),
    REG_NATIVE_FUNC(proc_raise, "(i)i"),
    REG_NATIVE_FUNC(random_get, "(*~)i"),
    REG_NATIVE_FUNC(sock_accept, "(i*)i"),
    REG_NATIVE_FUNC(sock_addr_local, "(i*i)i"),
    REG_NATIVE_FUNC(sock_addr_remote, "(i*i)i"),
    REG_NATIVE_FUNC(sock_addr_resolve, "(i*i*i)i"),
    REG_NATIVE_FUNC(sock_bind, "(i*)i"),
    REG_NATIVE_FUNC(sock_close, "(i)i"),
    REG_NATIVE_FUNC(sock_connect, "(i*)i"),
    REG_NATIVE_FUNC(sock_get_recv_buf_size, "(i*)i"),
    REG_NATIVE_FUNC(sock_get_reuse_addr, "(i*)i"),
    REG_NATIVE_FUNC(sock_get_reuse_port, "(i*)i"),
    REG_NATIVE_FUNC(sock_get_send_buf_size, "(i*)i"),
    REG_NATIVE_FUNC(sock_listen, "(ii)i"),
    REG_NATIVE_FUNC(sock_open, "(iii*)i"),
    REG_NATIVE_FUNC(sock_recv, "(i*ii**)i"),
    REG_NATIVE_FUNC(sock_send, "(i*ii*)i"),
    REG_NATIVE_FUNC(sock_set_recv_buf_size, "(ii)i"),
    REG_NATIVE_FUNC(sock_set_reuse_addr, "(ii)i"),
    REG_NATIVE_FUNC(sock_set_reuse_port, "(ii)i"),
    REG_NATIVE_FUNC(sock_set_send_buf_size, "(ii)i"),
    REG_NATIVE_FUNC(sock_recv, "(i*ii**)i"),
    REG_NATIVE_FUNC(sock_send, "(i*ii*)i"),
    REG_NATIVE_FUNC(sock_shutdown, "(ii)i"),
    REG_NATIVE_FUNC(sched_yield, "()i"),
};

uint32
get_libc_wasi_export_apis(NativeSymbol **p_libc_wasi_apis)
{
    *p_libc_wasi_apis = native_symbols_libc_wasi;
    return sizeof(native_symbols_libc_wasi) / sizeof(NativeSymbol);
}

int wasm_runtime_set_vsfwasi_args(wasm_module_t module,
                           char *argv[], int argc)
{
    if (__wasi_ctx_pls < 0) {
        __wasi_ctx_pls = vsf_linux_pls_alloc();
        if (__wasi_ctx_pls < 0) {
            printf("fail to allocate pls\n");
            return -1;
        }
    }

    vsf_linux_localstorage_t *pls = vsf_linux_pls_get(__wasi_ctx_pls);
    pls->destructor = free;
    pls->data = malloc(sizeof(wasm_ctx_t));
    if (NULL == pls->data) {
        printf("fail to allocate wasm_ctx\n");
        return -1;
    }

    wasm_ctx_t *wasm_ctx = pls->data;
    wasm_ctx->argc = argc;
    wasm_ctx->argv = argv;
    return 0;
}

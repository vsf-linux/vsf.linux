#ifdef __VSF__

// implement mingw_spawnvpe will require __VSF_LINUX_CLASS_IMPLEMENT
#define __VSF_LINUX_CLASS_IMPLEMENT
#define __VSF_LINUX_FS_CLASS_IMPLEMENT
#include <unistd.h>
#include <fcntl.h>
#include <spawn.h>

#define __VSF_HEADER_SHOW_CURL_CTX__
//#include "3rd-party/curl/raw/include/curl/curl.h"
#include "3rd-party/curl/raw/src/tool_setup.h"

#include "./git_port_vsf.h"

#if 1
struct tm *localtime_r(const time_t *t, struct tm *result)
{
    return NULL;
}

struct tm *gmtime_r(const time_t *t, struct tm *result)
{
    return NULL;
}

int ftruncate(int fildes, off_t length)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

pid_t tcgetpgrp(int fd)
{
    return -1;
}

int tcsetpgrp(int fd, pid_t pgrp)
{
    return -1;
}

pid_t mingw_spawnvpe(const char *cmd, const char **argv, char **deltaenv,
            const char *dir, int fhin, int fhout, int fherr)
{
    pid_t pid;
    posix_spawn_file_actions_t actions;
    posix_spawnattr_t attr;

    vsf_linux_process_t *process = vsf_linux_get_cur_process();

    posix_spawnattr_init(&attr);
    posix_spawn_file_actions_init(&actions);
    if (fhin != STDIN_FILENO) {
        posix_spawn_file_actions_adddup2(&actions, fhin, STDIN_FILENO);
    }
    if (fhout != STDOUT_FILENO) {
        posix_spawn_file_actions_adddup2(&actions, fhout, STDOUT_FILENO);
    }
    if (fherr != STDERR_FILENO) {
        posix_spawn_file_actions_adddup2(&actions, fherr, STDERR_FILENO);
    }

    // close all none-stdio fds
    __vsf_dlist_foreach_unsafe(vsf_linux_fd_t, fd_node, &process->fd_list) {
        if (!(_->fd_flags & FD_CLOEXEC) && (_->fd > 2)) {
            posix_spawn_file_actions_addclose(&actions, _->fd);
        }
    }

    posix_spawn(&pid, cmd, &actions, &attr, (char * const *)argv, deltaenv);

    posix_spawn_file_actions_destroy(&actions);
    posix_spawnattr_destroy(&attr);
    return pid;
}
#endif

static int __git_lib_idx = -1;

struct git_lib_ctx_t {
    struct curl_lib_ctx_t curl_lib_ctx;
    struct git_ctx_t __git_ctx;
};

#define git_lib_ctx         ((struct git_lib_ctx_t *)vsf_linux_library_ctx(__git_lib_idx))

int vsf_linux_git_init(void)
{
    struct git_lib_ctx_t *ctx = calloc(1, sizeof(struct git_lib_ctx_t));
    if (NULL == ctx) { return -1; }
    int err = vsf_linux_library_init(&__git_lib_idx, ctx);
    if (err) { return err; }
    err = curl_lib_init(&ctx->curl_lib_ctx);
    if (err) { return err; }

    // initializer

    return err;
}

void * __git_ctx(void)
{
    return &((struct git_lib_ctx_t *)vsf_linux_library_ctx(__git_lib_idx))->__git_ctx;
}

#endif      // __VSF__

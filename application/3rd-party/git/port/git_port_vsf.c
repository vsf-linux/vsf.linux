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

#undef git_ctx
    struct git_ctx_t *git_ctx = __git_ctx();

    // initializer
    extern void print_and_abort (void);
    git_ctx->obstack.__obstack_alloc_failed_handler = print_and_abort;
#if defined(HAVE_DEV_TTY) || defined(GIT_WINDOWS_NATIVE)
#   ifdef HAVE_DEV_TTY
    git_ctx->terminal.__term_fd = -1;
#   endif
    git_ctx->terminal.git_terminal_prompt.__buf = (struct strbuf)STRBUF_INIT;
#endif
    git_ctx->refs_debug.__trace_refs = (struct trace_key)TRACE_KEY_INIT(REFS);
#if defined(NO_MMAP)
    git_ctx->refs_packed_backend.__mmap_strategy = MMAP_NONE;
#elif defined(MMAP_PREVENTS_DELETE)
    git_ctx->refs_packed_backend.__mmap_strategy = MMAP_TEMPORARY;
#else
    git_ctx->refs_packed_backend.__mmap_strategy = MMAP_OK;
#endif
    git_ctx->refs_packed_backend.packed_refs_lock.__timeout_value = 1000;
    extern const struct ref_storage_be refs_be_files;
    git_ctx->refs.__refs_backends = &refs_be_files;
    git_ctx->refs.get_files_ref_lock_timeout_ms.__timeout_ms = 100;
    git_ctx->refs.refs_ref_iterator_begin.__ref_paranoia = -1;
    git_ctx->refs.refs_resolve_ref_unsafe.__sb_refname = (struct strbuf)STRBUF_INIT;
    git_ctx->tr2_cmd_name.__tr2cmdname_hierarchy = (struct strbuf)STRBUF_INIT;
    git_ctx->tr2_dst.tr2_dst_want_warning.__tr2env_dst_debug = -1;
    git_ctx->tr2_sid.__tr2sid_buf = (struct strbuf)STRBUF_INIT;

static struct tr2_sysenv_entry __tr2_sysenv_settings[] = {
    [TR2_SYSENV_CFG_PARAM]     = { "GIT_TRACE2_CONFIG_PARAMS",
                       "trace2.configparams" },
    [TR2_SYSENV_ENV_VARS]      = { "GIT_TRACE2_ENV_VARS",
                       "trace2.envvars" },

    [TR2_SYSENV_DST_DEBUG]     = { "GIT_TRACE2_DST_DEBUG",
                       "trace2.destinationdebug" },

    [TR2_SYSENV_NORMAL]        = { "GIT_TRACE2",
                       "trace2.normaltarget" },
    [TR2_SYSENV_NORMAL_BRIEF]  = { "GIT_TRACE2_BRIEF",
                       "trace2.normalbrief" },

    [TR2_SYSENV_EVENT]         = { "GIT_TRACE2_EVENT",
                       "trace2.eventtarget" },
    [TR2_SYSENV_EVENT_BRIEF]   = { "GIT_TRACE2_EVENT_BRIEF",
                       "trace2.eventbrief" },
    [TR2_SYSENV_EVENT_NESTING] = { "GIT_TRACE2_EVENT_NESTING",
                       "trace2.eventnesting" },

    [TR2_SYSENV_PERF]          = { "GIT_TRACE2_PERF",
                       "trace2.perftarget" },
    [TR2_SYSENV_PERF_BRIEF]    = { "GIT_TRACE2_PERF_BRIEF",
                       "trace2.perfbrief" },

    [TR2_SYSENV_MAX_FILES]     = { "GIT_TRACE2_MAX_FILES",
                       "trace2.maxfiles" },
};
    if (dimof(__tr2_sysenv_settings) > dimof(git_ctx->tr2_sysenv.__tr2_sysenv_settings)) {
        VSF_LINUX_ASSERT(false);
        return -1;
    }
    memcpy(git_ctx->tr2_sysenv.__tr2_sysenv_settings, __tr2_sysenv_settings, sizeof(__tr2_sysenv_settings));

    git_ctx->__tr2_tgt_event.__tr2dst_event = (struct tr2_dst){ TR2_SYSENV_EVENT, 0, 0, 0, 0 };
    git_ctx->__tr2_tgt_event.__tr2env_event_max_nesting_levels = 2;
    git_ctx->__tr2_tgt_event.__tr2_tgt_event = __tr2_tgt_event;
    git_ctx->__tr2_tgt_event.__tr2_tgt_event.pdst = &git_ctx->__tr2_tgt_event.__tr2dst_event;
    git_ctx->__tr2_tgt_normal.__tr2dst_normal = (struct tr2_dst){ TR2_SYSENV_NORMAL, 0, 0, 0, 0 };
    git_ctx->__tr2_tgt_normal.__tr2_tgt_normal = __tr2_tgt_normal;
    git_ctx->__tr2_tgt_normal.__tr2_tgt_normal.pdst = &git_ctx->__tr2_tgt_normal.__tr2dst_normal;
    git_ctx->__tr2_tgt_perf.__tr2dst_perf = (struct tr2_dst){ TR2_SYSENV_PERF, 0, 0, 0, 0 };
    git_ctx->__tr2_tgt_perf.__tr2_tgt_perf = __tr2_tgt_perf;
    git_ctx->__tr2_tgt_perf.__tr2_tgt_perf.pdst = &git_ctx->__tr2_tgt_perf.__tr2dst_perf;
    git_ctx->trace2.__tr2_next_repo_id = 1;

struct tr2_tgt *__tr2_tgt_builtins[] =
{
    &tr2_tgt_normal,
    &tr2_tgt_perf,
    &tr2_tgt_event,
    NULL
};
    if (dimof(__tr2_tgt_builtins) > dimof(git_ctx->trace2.__tr2_tgt_builtins)) {
        VSF_LINUX_ASSERT(false);
        return -1;
    }
    memcpy(git_ctx->trace2.__tr2_tgt_builtins, __tr2_tgt_builtins, sizeof(__tr2_tgt_builtins));

    return err;
}

void * __git_ctx(void)
{
    return &((struct git_lib_ctx_t *)vsf_linux_library_ctx(__git_lib_idx))->__git_ctx;
}

#endif      // __VSF__

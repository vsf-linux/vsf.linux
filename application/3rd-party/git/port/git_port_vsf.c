#ifdef __VSF__

#if 1
// implement mingw_spawnvpe will require __VSF_LINUX_CLASS_IMPLEMENT
#define __VSF_LINUX_CLASS_IMPLEMENT
#define __VSF_LINUX_FS_CLASS_IMPLEMENT
#include <unistd.h>
#include <fcntl.h>
#include <spawn.h>

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

#endif      // __VSF__

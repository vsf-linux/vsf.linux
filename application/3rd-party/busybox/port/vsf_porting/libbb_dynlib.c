#ifdef __VSF__

#include <syslog.h>
#include "libbb.h"

#define define_vsf_busybox_mod(__mod_name, __mod_size, __mod_idx, __mod_init)   \
        const vsf_linux_dynlib_mod_t vsf_busybox_mod_name(__mod_name) = {       \
            .lib_idx            = NULL,                                         \
            .mod_idx            = (__mod_idx),                                  \
            .module_num         = 1,                                            \
            .bss_size           = (__mod_size),                                 \
            .mod_size           = (__mod_size),                                 \
            .init               = (__mod_init),                                 \
        };

static void __busybox_libbb_init(void *ctx)
{
    struct __busybox_libbb_ctx *mod_ctx = ctx;

    xfunc_error_retval = EXIT_FAILURE;
    logmode = LOGMODE_STDIO;
    msg_eol = "\n";

    // init
    mod_ctx->init.message.log_fd = -1;
    // verror_msg.c
    mod_ctx->verror_msg.syslog_level = LOG_ERR;
}
define_vsf_busybox_mod(libbb, sizeof(struct __busybox_libbb_ctx), VSF_BUSYBOX_MOD_LIBBB, __busybox_libbb_init)
define_vsf_busybox_mod(libbb_fork, 0, VSF_BUSYBOX_MOD_LIBBB, NULL)

#endif      // __VSF__

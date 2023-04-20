#ifdef __VSF__

#include <syslog.h>
#include "libbb.h"

int vsf_busybox_dynlib_idx = -1;

static void __busybox_libbb_init(void *ctx)
{
    struct __busybox_libbb_ctx *mod_ctx = ctx;

    xfunc_error_retval = EXIT_FAILURE;
    logmode = LOGMODE_STDIO;
    msg_eol = "\n";

    // verror_msg.c
    mod_ctx->verror_msg.syslog_level = LOG_ERR;
}
define_vsf_busybox_mod(libbb, sizeof(struct __busybox_libbb_ctx), VSF_BUSYBOX_MOD_LIBBB, __busybox_libbb_init)

#endif      // __VSF__

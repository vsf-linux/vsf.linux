#ifdef __VSF__

#define __VSF_HEADER_SHOW_NCURSES_CTX__
#include "./ncurses_port_vsf.h"

static int __ncurses_lib_idx = -1;

#define ncurses_lib_ctx     ((struct ncurses_lib_ctx_t *)vsf_linux_library_ctx(__ncurses_lib_idx))

int ncurses_lib_init(struct ncurses_lib_ctx_t *ctx)
{
    int err = vsf_linux_library_init(&__ncurses_lib_idx, ctx, NULL);
    if (!err) {
        // initialize
        ncurses_ctx->__TABSIZE = 8;
        ncurses_ctx->__ESCDELAY = 1000;
        ncurses_tic_ctx->comp_scan.___nc_strict_bsd = 1;

#define CHARS_0s { '\0' }

#define TGETENT_0 { 0L, FALSE, NULL, NULL, NULL }
#define TGETENT_0s { TGETENT_0, TGETENT_0, TGETENT_0, TGETENT_0 }

        _nc_globals = (NCURSES_EXPORT_VAR(NCURSES_GLOBALS)) {
    0,				/* have_sigtstp */
    0,				/* have_sigwinch */
    0,				/* cleanup_nested */

    FALSE,			/* init_signals */
    FALSE,			/* init_screen */

    NULL,			/* comp_sourcename */
    NULL,			/* comp_termtype */

    FALSE,			/* have_tic_directory */
    FALSE,			/* keep_tic_directory */
    0,				/* tic_directory */

    NULL,			/* dbi_list */
    0,				/* dbi_size */

    NULL,			/* first_name */
    NULL,			/* keyname_table */
    0,				/* init_keyname */

    0,				/* slk_format */

    2048,			/* getstr_limit */

    NULL,			/* safeprint_buf */
    0,				/* safeprint_used */

    TGETENT_0s,			/* tgetent_cache */
    0,				/* tgetent_index */
    0,				/* tgetent_sequence */

    0,				/* dbd_blob */
    0,				/* dbd_list */
    0,				/* dbd_size */
    0,				/* dbd_time */
    { { 0, 0 } },		/* dbd_vars */

#if HAVE_TSEARCH
    NULL,			/* cached_tparm */
    0,				/* count_tparm */
#endif /* HAVE_TSEARCH */

#ifdef USE_TERM_DRIVER
    0,				/* term_driver */
#endif

#ifndef USE_SP_WINDOWLIST
    0,				/* _nc_windowlist */
#endif

#if USE_HOME_TERMINFO
    NULL,			/* home_terminfo */
#endif

#if !USE_SAFE_SPRINTF
    0,				/* safeprint_cols */
    0,				/* safeprint_rows */
#endif

#ifdef USE_PTHREADS
    PTHREAD_MUTEX_INITIALIZER,	/* mutex_curses */
    PTHREAD_MUTEX_INITIALIZER,	/* mutex_prescreen */
    PTHREAD_MUTEX_INITIALIZER,	/* mutex_screen */
    PTHREAD_MUTEX_INITIALIZER,	/* mutex_update */
    PTHREAD_MUTEX_INITIALIZER,	/* mutex_tst_tracef */
    PTHREAD_MUTEX_INITIALIZER,	/* mutex_tracef */
    0,				/* nested_tracef */
    0,				/* use_pthreads */
#if USE_PTHREADS_EINTR
    0,				/* read_thread */
#endif
#endif
#if USE_WIDEC_SUPPORT
    CHARS_0s,			/* key_name */
#endif
#ifdef TRACE
    FALSE,			/* trace_opened */
    CHARS_0s,			/* trace_fname */
    0,				/* trace_level */
    NULL,			/* trace_fp */
    -1,				/* trace_fd */

    NULL,			/* tracearg_buf */
    0,				/* tracearg_used */

    NULL,			/* tracebuf_ptr */
    0,				/* tracebuf_used */

    CHARS_0s,			/* tracechr_buf */

    NULL,			/* tracedmp_buf */
    0,				/* tracedmp_used */

    NULL,			/* tracetry_buf */
    0,				/* tracetry_used */

    { CHARS_0s, CHARS_0s },	/* traceatr_color_buf */
    0,				/* traceatr_color_sel */
    -1,				/* traceatr_color_last */
#if !defined(USE_PTHREADS) && USE_REENTRANT
    0,				/* nested_tracef */
#endif
#endif /* TRACE */
#if NO_LEAKS
    FALSE,			/* leak_checking */
#endif
};

#define STACK_FRAME_0	{ { 0 }, 0 }
#define STACK_FRAME_0s	{ STACK_FRAME_0 }
#define NUM_VARS_0s	{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 }

#define RIPOFF_0	{ 0,0,0 }
#define RIPOFF_0s	{ RIPOFF_0 }

        _nc_prescreen = (NCURSES_EXPORT_VAR(NCURSES_PRESCREEN)) {
    NULL,			/* allocated */
    TRUE,			/* use_env */
    FALSE,			/* filter_mode */
    A_NORMAL,			/* previous_attr */
    {				/* tparm_state */
	NULL,			/* tparam_base */

	STACK_FRAME_0s,		/* stack */
	0,			/* stack_ptr */

	NULL,			/* out_buff */
	0,			/* out_size */
	0,			/* out_used */

	NULL,			/* fmt_buff */
	0,			/* fmt_size */

	NUM_VARS_0s,		/* static_vars */
#ifdef TRACE
	NULL,			/* tname */
#endif
    },
    NULL,			/* saved_tty */
    FALSE,			/* use_tioctl */
    0,				/* _outch */
#ifndef USE_SP_RIPOFF
    RIPOFF_0s,			/* ripoff */
    NULL,			/* rsp */
#endif
#if NCURSES_NO_PADDING
    FALSE,			/* flag to set if padding disabled  */
#endif
#if BROKEN_LINKER || USE_REENTRANT
    NULL,			/* real_acs_map */
    0,				/* LINES */
    0,				/* COLS */
    8,				/* TABSIZE */
    1000,			/* ESCDELAY */
    0,				/* cur_term */
#endif
#ifdef TRACE
#if BROKEN_LINKER || USE_REENTRANT
    0L,				/* _outchars */
    NULL,			/* _tputs_trace */
#endif
#endif
};
    }
    return err;
}

void * __ncurses_ctx(void)
{
    return (void *)&ncurses_lib_ctx->__ncurses_ctx;
}

void * __ncurses_tic_ctx(void)
{
    return (void *)&ncurses_lib_ctx->__tic_ctx;
}

void * __ncurses_new_pair_ctx(void)
{
    return (void *)&ncurses_lib_ctx->__new_pair_ctx;
}

void * __ncurses_termcap_ctx(void)
{
    return (void *)&ncurses_lib_ctx->__termcap_ctx;
}

#endif      // __VSF__

enum { COMMON_BUFSIZE = 1024 };
#ifdef __VSF__
# define bb_common_bufsiz1 (libbb_ctx->bb_common_bufsiz1)
#else
extern char bb_common_bufsiz1[];
#endif
#define setup_common_bufsiz() ((void)0)

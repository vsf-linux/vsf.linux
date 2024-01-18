enum { COMMON_BUFSIZE = 1024 };
#if defined(__VSF__) && !defined(__VSF_APPLET__)
# define bb_common_bufsiz1 (libbb_ctx->bb_common_bufsiz1)
#else
extern char bb_common_bufsiz1[];
#endif
#define setup_common_bufsiz() memset(bb_common_bufsiz1, 0, COMMON_BUFSIZE)

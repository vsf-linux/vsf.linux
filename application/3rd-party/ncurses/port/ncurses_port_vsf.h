#ifdef __VSF_HEADER_SHOW_NCURSES_CTX__
#undef __VSF_HEADER_SHOW_NCURSES_CTX__

#define NEW_PAIR_INTERNAL
#include <curses.priv.h>
#include <new_pair.h>
#include <termcap.h>
#include <tic.h>

struct ncurses_lib_ctx_t {
    struct ncurses_ctx_t __ncurses_ctx;
    struct ncurses_tic_ctx_t __tic_ctx;
    struct ncurses_new_pair_ctx_t __new_pair_ctx;
    struct ncurses_termcap_ctx_t __termcap_ctx;
};

extern int ncurses_lib_init(struct ncurses_lib_ctx_t *ctx);
#endif      // __VSF_HEADER_SHOW_NCURSES_CTX__

void * __ncurses_ctx(void);
#define ncurses_ctx             ((struct ncurses_ctx_t *)__ncurses_ctx())

void * __ncurses_tic_ctx(void);
#define ncurses_tic_ctx         ((struct ncurses_tic_ctx_t *)__ncurses_tic_ctx())

void * __ncurses_new_pair_ctx(void);
#define ncurses_new_pair_ctx    ((struct ncurses_new_pair_ctx_t *)__ncurses_new_pair_ctx())

void * __ncurses_termcap_ctx(void);
#define ncurses_termcap_ctx     ((struct ncurses_termcap_ctx_t *)__ncurses_termcap_ctx())

#ifdef __VSF__

#include "less.h"
#include "./less_port_vsf.h"

// in defines.h, clear/winch are mapped to less_clear/less_winch to avoid confliction
#undef clear
#undef winch

#include "cmd.h"
#include "option.h"

struct less_lib_ctx_t {
    struct less_ctx_t __less_ctx;
};

static int __less_lib_idx = -1;

int vsf_linux_less_init(void)
{
    struct less_lib_ctx_t *ctx = calloc(1, sizeof(struct less_lib_ctx_t));
    if (NULL == ctx) { return -1; }
    int err = vsf_linux_library_init(&__less_lib_idx, ctx, free);
    if (err) { return err; }

    // initialize
    less_ctx->pub.__binattr = AT_STANDOUT|AT_COLOR_BIN;
    less_ctx->pub.__updown_match = -1;
    less_ctx->pub.__openquote = '"';
    less_ctx->pub.__closequote = '"';
#if CMD_HISTORY
    less_ctx->pub.__ml_search = (void *) &less_ctx->__cmdbuf.__mlist_search;
    less_ctx->pub.__ml_examine = (void *) &less_ctx->__cmdbuf.__mlist_examine;
#if SHELL_ESCAPE || PIPEC
    less_ctx->pub.__ml_shell = (void *) &less_ctx->__cmdbuf.__mlist_shell;
#endif
#else
    less_ctx->pub.__ml_search = (void *)1;
    less_ctx->pub.__ml_examine = (void *)2;
#if SHELL_ESCAPE || PIPEC
    less_ctx->pub.__ml_shell = (void *)3;
#endif
#endif
    less_ctx->pub.__first_time = 1;
    less_ctx->pub.__ntabstops = 1;
    less_ctx->pub.__tabdefault = 8;
    less_ctx->pub.__start_attnpos = NULL_POSITION;
    less_ctx->pub.__end_attnpos = NULL_POSITION;
#if LOGFILE
    less_ctx->pub.__logfile = -1;
#endif
    less_ctx->pub.__jump_sline_fraction = -1;
    less_ctx->pub.__shift_count_fraction = -1;
    extern constant char e_proto[];
    extern constant char h_proto[];
    extern constant char w_proto[];
    less_ctx->pub.__eqproto = e_proto;
    less_ctx->pub.__hproto = h_proto;
    less_ctx->pub.__wproto = w_proto;
#if TAGS
    extern char ztags[];
    less_ctx->pub.__tags = ztags;
#endif
    less_ctx->pub.__jump_sline_fraction = -1;
    less_ctx->pub.__shift_count_fraction = -1;

    less_ctx->ch.__ch_ungotchar = -1;
    less_ctx->ch.__maxbufs = -1;
    less_ctx->__cmdbuf.__mlist_search = (struct mlist){ &less_ctx->__cmdbuf.__mlist_search,  &less_ctx->__cmdbuf.__mlist_search,  &less_ctx->__cmdbuf.__mlist_search,  NULL, 0 };
    less_ctx->__cmdbuf.__mlist_examine = (struct mlist){ &less_ctx->__cmdbuf.__mlist_examine, &less_ctx->__cmdbuf.__mlist_examine, &less_ctx->__cmdbuf.__mlist_examine, NULL, 0 };
#if SHELL_ESCAPE || PIPEC
    less_ctx->__cmdbuf.__mlist_shell = (struct mlist){ &less_ctx->__cmdbuf.__mlist_shell,   &less_ctx->__cmdbuf.__mlist_shell,   &less_ctx->__cmdbuf.__mlist_shell,   NULL, 0 };
#endif
    less_ctx->ifile.__anchor = (struct ifile){ &less_ctx->ifile.__anchor, &less_ctx->ifile.__anchor, NULL, NULL, NULL, 0, 0, '\0', { NULL_POSITION, 0 } };
//    less_ctx->lesskey_parser.tstr.__tstr_control_k = (char [7]){ SK_SPECIAL_KEY, SK_CONTROL_K, 6, 1, 1, 1, '\0' };
    memcpy(less_ctx->lesskey_parser.tstr.__tstr_control_k, (char [7]){ SK_SPECIAL_KEY, SK_CONTROL_K, 6, 1, 1, 1, '\0' }, 7);
//    less_ctx->line.ansi_step.__hlink_prefix = ESCS "]8;";
    memcpy(less_ctx->line.ansi_step.__hlink_prefix, ESCS "]8;", 5);
    less_ctx->output.__ob = less_ctx->output.__obuf;
    less_ctx->output.__outfd = 2;
    less_ctx->screen.__attrcolor = -1;
    less_ctx->screen.__termcap_debug = -1;
#if TAGS
    less_ctx->tags.__taglist = { ((struct tag *) &less_ctx->tags.__taglist), ((struct tag *) &less_ctx->tags.__taglist) };
#endif

#   define utf_mode             (less_ctx->pub.__utf_mode)
struct charset __charsets[] = {
        { "ascii",              NULL,       "8bcccbcc18b95.b" },
        { "utf-8",              &utf_mode,  "8bcccbcc18b95.b126.bb" },
        { "iso8859",            NULL,       "8bcccbcc18b95.33b." },
        { "latin3",             NULL,       "8bcccbcc18b95.33b5.b8.b15.b4.b12.b18.b12.b." },
        { "arabic",             NULL,       "8bcccbcc18b95.33b.3b.7b2.13b.3b.b26.5b19.b" },
        { "greek",              NULL,       "8bcccbcc18b95.33b4.2b4.b3.b35.b44.b" },
        { "greek2005",          NULL,       "8bcccbcc18b95.33b14.b35.b44.b" },
        { "hebrew",             NULL,       "8bcccbcc18b95.33b.b29.32b28.2b2.b" },
        { "koi8-r",             NULL,       "8bcccbcc18b95.b." },
        { "KOI8-T",             NULL,       "8bcccbcc18b95.b8.b6.b8.b.b.5b7.3b4.b4.b3.b.b.3b." },
        { "georgianps",         NULL,       "8bcccbcc18b95.3b11.4b12.2b." },
        { "tcvn",               NULL,       "b..b...bcccbccbbb7.8b95.b48.5b." },
        { "TIS-620",            NULL,       "8bcccbcc18b95.b.4b.11b7.8b." },
        { "next",               NULL,       "8bcccbcc18b95.bb125.bb" },
        { "dos",                NULL,       "8bcccbcc12bc5b95.b." },
        { "windows-1251",       NULL,       "8bcccbcc12bc5b95.b24.b." },
        { "windows-1252",       NULL,       "8bcccbcc12bc5b95.b.b11.b.2b12.b." },
        { "windows-1255",       NULL,       "8bcccbcc12bc5b95.b.b8.b.5b9.b.4b." },
        { "ebcdic",             NULL,       "5bc6bcc7bcc41b.9b7.9b5.b..8b6.10b6.b9.7b9.8b8.17b3.3b9.7b9.8b8.6b10.b.b.b." },
        { "IBM-1047",           NULL,       "4cbcbc3b9cbccbccbb4c6bcc5b3cbbc4bc4bccbc191.b" },
        { NULL, NULL, NULL }
};
    if (sizeof(__charsets) > sizeof(less_ctx->charset.__charsets)) {
        fprintf(stderr, "not enough space in less_ctx->charset.__charsets, need %d\n", dimof(__charsets));
        VSF_LINUX_ASSERT(false);
        return -1;
    }
    memcpy(less_ctx->charset.__charsets, __charsets, sizeof(__charsets));

#define SK(k) \
    SK_SPECIAL_KEY, (k), 6, 1, 1, 1
static const unsigned char __cmdtable[] =
{
    '\r',0,                         A_F_LINE,
    '\n',0,                         A_F_LINE,
    'e',0,                          A_F_LINE,
    'j',0,                          A_F_LINE,
    SK(SK_DOWN_ARROW),0,            A_F_LINE,
    CONTROL('E'),0,                 A_F_LINE,
    CONTROL('N'),0,                 A_F_LINE,
    'k',0,                          A_B_LINE,
    'y',0,                          A_B_LINE,
    CONTROL('Y'),0,                 A_B_LINE,
    SK(SK_CONTROL_K),0,             A_B_LINE,
    CONTROL('P'),0,                 A_B_LINE,
    SK(SK_UP_ARROW),0,              A_B_LINE,
    'J',0,                          A_FF_LINE,
    'K',0,                          A_BF_LINE,
    'Y',0,                          A_BF_LINE,
    'd',0,                          A_F_SCROLL,
    CONTROL('D'),0,                 A_F_SCROLL,
    'u',0,                          A_B_SCROLL,
    CONTROL('U'),0,                 A_B_SCROLL,
    ESC,'[','M',0,                  A_X11MOUSE_IN,
    ESC,'[','<',0,                  A_X116MOUSE_IN,
    ' ',0,                          A_F_SCREEN,
    'f',0,                          A_F_SCREEN,
    CONTROL('F'),0,                 A_F_SCREEN,
    CONTROL('V'),0,                 A_F_SCREEN,
    SK(SK_PAGE_DOWN),0,             A_F_SCREEN,
    'b',0,                          A_B_SCREEN,
    CONTROL('B'),0,                 A_B_SCREEN,
    ESC,'v',0,                      A_B_SCREEN,
    SK(SK_PAGE_UP),0,               A_B_SCREEN,
    'z',0,                          A_F_WINDOW,
    'w',0,                          A_B_WINDOW,
    ESC,' ',0,                      A_FF_SCREEN,
    'F',0,                          A_F_FOREVER,
    ESC,'F',0,                      A_F_UNTIL_HILITE,
    'R',0,                          A_FREPAINT,
    'r',0,                          A_REPAINT,
    CONTROL('R'),0,                 A_REPAINT,
    CONTROL('L'),0,                 A_REPAINT,
    ESC,'u',0,                      A_UNDO_SEARCH,
    ESC,'U',0,                      A_CLR_SEARCH,
    'g',0,                          A_GOLINE,
    SK(SK_HOME),0,                  A_GOLINE,
    '<',0,                          A_GOLINE,
    ESC,'<',0,                      A_GOLINE,
    'p',0,                          A_PERCENT,
    '%',0,                          A_PERCENT,
    ESC,'[',0,                      A_LSHIFT,
    ESC,']',0,                      A_RSHIFT,
    ESC,'(',0,                      A_LSHIFT,
    ESC,')',0,                      A_RSHIFT,
    ESC,'{',0,                      A_LLSHIFT,
    ESC,'}',0,                      A_RRSHIFT,
    SK(SK_RIGHT_ARROW),0,           A_RSHIFT,
    SK(SK_LEFT_ARROW),0,            A_LSHIFT,
    SK(SK_CTL_RIGHT_ARROW),0,       A_RRSHIFT,
    SK(SK_CTL_LEFT_ARROW),0,        A_LLSHIFT,
    '{',0,                          A_F_BRACKET|A_EXTRA,        '{','}',0,
    '}',0,                          A_B_BRACKET|A_EXTRA,        '{','}',0,
    '(',0,                          A_F_BRACKET|A_EXTRA,        '(',')',0,
    ')',0,                          A_B_BRACKET|A_EXTRA,        '(',')',0,
    '[',0,                          A_F_BRACKET|A_EXTRA,        '[',']',0,
    ']',0,                          A_B_BRACKET|A_EXTRA,        '[',']',0,
    ESC,CONTROL('F'),0,             A_F_BRACKET,
    ESC,CONTROL('B'),0,             A_B_BRACKET,
    'G',0,                          A_GOEND,
    ESC,'G',0,                      A_GOEND_BUF,
    ESC,'>',0,                      A_GOEND,
    '>',0,                          A_GOEND,
    SK(SK_END),0,                   A_GOEND,
    'P',0,                          A_GOPOS,

    '0',0,                          A_DIGIT,
    '1',0,                          A_DIGIT,
    '2',0,                          A_DIGIT,
    '3',0,                          A_DIGIT,
    '4',0,                          A_DIGIT,
    '5',0,                          A_DIGIT,
    '6',0,                          A_DIGIT,
    '7',0,                          A_DIGIT,
    '8',0,                          A_DIGIT,
    '9',0,                          A_DIGIT,
    '.',0,                          A_DIGIT,

    '=',0,                          A_STAT,
    CONTROL('G'),0,                 A_STAT,
    ':','f',0,                      A_STAT,
    '/',0,                          A_F_SEARCH,
    '?',0,                          A_B_SEARCH,
    ESC,'/',0,                      A_F_SEARCH|A_EXTRA,        '*',0,
    ESC,'?',0,                      A_B_SEARCH|A_EXTRA,        '*',0,
    'n',0,                          A_AGAIN_SEARCH,
    ESC,'n',0,                      A_T_AGAIN_SEARCH,
    'N',0,                          A_REVERSE_SEARCH,
    ESC,'N',0,                      A_T_REVERSE_SEARCH,
    '&',0,                          A_FILTER,
    'm',0,                          A_SETMARK,
    'M',0,                          A_SETMARKBOT,
    ESC,'m',0,                      A_CLRMARK,
    '\'',0,                         A_GOMARK,
    CONTROL('X'),CONTROL('X'),0,    A_GOMARK,
    'E',0,                          A_EXAMINE,
    ':','e',0,                      A_EXAMINE,
    CONTROL('X'),CONTROL('V'),0,    A_EXAMINE,
    ':','n',0,                      A_NEXT_FILE,
    ':','p',0,                      A_PREV_FILE,
    't',0,                          A_NEXT_TAG,
    'T',0,                          A_PREV_TAG,
    ':','x',0,                      A_INDEX_FILE,
    ':','d',0,                      A_REMOVE_FILE,
    '-',0,                          A_OPT_TOGGLE,
    ':','t',0,                      A_OPT_TOGGLE|A_EXTRA,        't',0,
    's',0,                          A_OPT_TOGGLE|A_EXTRA,        'o',0,
    '_',0,                          A_DISP_OPTION,
    '|',0,                          A_PIPE,
    'v',0,                          A_VISUAL,
    '!',0,                          A_SHELL,
    '+',0,                          A_FIRSTCMD,

    'H',0,                          A_HELP,
    'h',0,                          A_HELP,
    SK(SK_F1),0,                    A_HELP,
    'V',0,                          A_VERSION,
    'q',0,                          A_QUIT,
    'Q',0,                          A_QUIT,
    ':','q',0,                      A_QUIT,
    ':','Q',0,                      A_QUIT,
    'Z','Z',0,                      A_QUIT
};

static const unsigned char __edittable[] =
{
    '\t',0,                         EC_F_COMPLETE,  /* TAB */
    '\17',0,                        EC_B_COMPLETE,  /* BACKTAB */
    SK(SK_BACKTAB),0,               EC_B_COMPLETE,  /* BACKTAB */
    ESC,'\t',0,                     EC_B_COMPLETE,  /* ESC TAB */
    CONTROL('L'),0,                 EC_EXPAND,      /* CTRL-L */
    CONTROL('V'),0,                 EC_LITERAL,     /* BACKSLASH */
    CONTROL('A'),0,                 EC_LITERAL,     /* BACKSLASH */
    ESC,'l',0,                      EC_RIGHT,       /* ESC l */
    SK(SK_RIGHT_ARROW),0,           EC_RIGHT,       /* RIGHTARROW */
    ESC,'h',0,                      EC_LEFT,        /* ESC h */
    SK(SK_LEFT_ARROW),0,            EC_LEFT,        /* LEFTARROW */
    ESC,'b',0,                      EC_W_LEFT,      /* ESC b */
    ESC,SK(SK_LEFT_ARROW),0,        EC_W_LEFT,      /* ESC LEFTARROW */
    SK(SK_CTL_LEFT_ARROW),0,        EC_W_LEFT,      /* CTRL-LEFTARROW */
    ESC,'w',0,                      EC_W_RIGHT,     /* ESC w */
    ESC,SK(SK_RIGHT_ARROW),0,       EC_W_RIGHT,     /* ESC RIGHTARROW */
    SK(SK_CTL_RIGHT_ARROW),0,       EC_W_RIGHT,     /* CTRL-RIGHTARROW */
    ESC,'i',0,                      EC_INSERT,      /* ESC i */
    SK(SK_INSERT),0,                EC_INSERT,      /* INSERT */
    ESC,'x',0,                      EC_DELETE,      /* ESC x */
    SK(SK_DELETE),0,                EC_DELETE,      /* DELETE */
    ESC,'X',0,                      EC_W_DELETE,    /* ESC X */
    ESC,SK(SK_DELETE),0,            EC_W_DELETE,    /* ESC DELETE */
    SK(SK_CTL_DELETE),0,            EC_W_DELETE,    /* CTRL-DELETE */
    SK(SK_CTL_BACKSPACE),0,         EC_W_BACKSPACE, /* CTRL-BACKSPACE */
    ESC,SK(SK_BACKSPACE),0,         EC_W_BACKSPACE, /* ESC BACKSPACE */
    ESC,'0',0,                      EC_HOME,        /* ESC 0 */
    SK(SK_HOME),0,                  EC_HOME,        /* HOME */
    ESC,'$',0,                      EC_END,         /* ESC $ */
    SK(SK_END),0,                   EC_END,         /* END */
    ESC,'k',0,                      EC_UP,          /* ESC k */
    SK(SK_UP_ARROW),0,              EC_UP,          /* UPARROW */
    ESC,'j',0,                      EC_DOWN,        /* ESC j */
    SK(SK_DOWN_ARROW),0,            EC_DOWN,        /* DOWNARROW */
    CONTROL('G'),0,                 EC_ABORT,       /* CTRL-G */
    ESC,'[','M',0,                  EC_X11MOUSE,    /* X11 mouse report */
    ESC,'[','<',0,                  EC_X116MOUSE,   /* X11 1006 mouse report */
};
    if (sizeof(__cmdtable) > sizeof(less_ctx->decode.__cmdtable)) {
        fprintf(stderr, "not enough space in less_ctx->decode.__cmdtable, need %d\n", dimof(__cmdtable));
        VSF_LINUX_ASSERT(false);
        return -1;
    }
    memcpy(less_ctx->decode.__cmdtable, __cmdtable, sizeof(__cmdtable));
    if (sizeof(__edittable) > sizeof(less_ctx->decode.__edittable)) {
        fprintf(stderr, "not enough space in less_ctx->decode.__edittable, need %d\n", dimof(__edittable));
        VSF_LINUX_ASSERT(false);
        return -1;
    }
    memcpy(less_ctx->decode.__edittable, __edittable, sizeof(__edittable));

extern const struct optname a_optname;
extern const struct optname b_optname;
extern const struct optname B__optname;
extern const struct optname c_optname;
extern const struct optname d_optname;
extern const struct optname D__optname;
extern const struct optname e_optname;
extern const struct optname f_optname;
extern const struct optname F__optname;
#if HILITE_SEARCH
extern const struct optname g_optname;
#endif
extern const struct optname h_optname;
extern const struct optname i_optname;
extern const struct optname j_optname;
extern const struct optname J__optname;
#if USERFILE
extern const struct optname k_optname;
#if HAVE_LESSKEYSRC 
extern const struct optname ks_optname;
#endif /* HAVE_LESSKEYSRC */
#endif
extern const struct optname K__optname;
extern const struct optname L__optname;
extern const struct optname m_optname;
extern const struct optname n_optname;
#if LOGFILE
extern const struct optname o_optname;
extern const struct optname O__optname;
#endif
extern const struct optname p_optname;
extern const struct optname P__optname;
extern const struct optname q2_optname;
extern const struct optname q_optname;
extern const struct optname r_optname;
extern const struct optname s_optname;
extern const struct optname S__optname;
#if TAGS
extern const struct optname t_optname;
extern const struct optname T__optname;
#endif
extern const struct optname u_optname;
extern const struct optname V__optname;
extern const struct optname w_optname;
extern const struct optname x_optname;
extern const struct optname X__optname;
extern const struct optname y_optname;
extern const struct optname z_optname;
extern const struct optname quote_optname;
extern const struct optname tilde_optname;
extern const struct optname query_optname;
extern const struct optname pound_optname;
extern const struct optname keypad_optname;
extern const struct optname oldbot_optname;
extern const struct optname follow_optname;
extern const struct optname use_backslash_optname;
extern const struct optname rscroll_optname;
extern const struct optname nohistdups_optname;
extern const struct optname mousecap_optname;
extern const struct optname wheel_lines_optname;
extern const struct optname perma_marks_optname;
extern const struct optname linenum_width_optname;
extern const struct optname status_col_width_optname;
extern const struct optname incr_search_optname;
extern const struct optname use_color_optname;
extern const struct optname want_filesize_optname;
extern const struct optname status_line_optname;
extern const struct optname header_optname;
extern const struct optname nonum_headers_optname;
extern const struct optname redraw_on_quit_optname;
extern const struct optname search_type_optname;
#if LESSTEST
extern const struct optname ttyin_name_optname;
extern const struct optname rstat_optname;
#endif /*LESSTEST*/
#define how_search              (less_ctx->pub.__how_search)
#define bufspace                (less_ctx->pub.__bufspace)
#define autobuf                 (less_ctx->pub.__autobuf)
#define top_scroll              (less_ctx->pub.__top_scroll)
#define know_dumb               (less_ctx->pub.__know_dumb)
#define quit_at_eof             (less_ctx->pub.__quit_at_eof)
#define force_open              (less_ctx->pub.__force_open)
#define quit_if_one_screen      (less_ctx->pub.__quit_if_one_screen)
#if HILITE_SEARCH
#define hilite_search           (less_ctx->pub.__hilite_search)
#endif
#define back_scroll             (less_ctx->pub.__back_scroll)
#define caseless                (less_ctx->pub.__caseless)
#define status_col              (less_ctx->pub.__status_col)
#define quit_on_intr            (less_ctx->pub.__quit_on_intr)
#define use_lessopen            (less_ctx->pub.__use_lessopen)
#define pr_type                 (less_ctx->pub.__pr_type)
#define linenums                (less_ctx->pub.__linenums)
#define quiet                   (less_ctx->pub.__quiet)
#define ctldisp                 (less_ctx->pub.__ctldisp)
#define squeeze                 (less_ctx->pub.__squeeze)
#define chopline                (less_ctx->pub.__chopline)
#define bs_mode                 (less_ctx->pub.__bs_mode)
#define show_attn               (less_ctx->pub.__show_attn)
#define no_init                 (less_ctx->pub.__no_init)
#define forw_scroll             (less_ctx->pub.__forw_scroll)
#define swindow                 (less_ctx->pub.__swindow)
#define twiddle                 (less_ctx->pub.__twiddle)
#define no_keypad               (less_ctx->pub.__no_keypad)
#define oldbot                  (less_ctx->pub.__oldbot)
#define follow_mode             (less_ctx->pub.__follow_mode)
#define opt_use_backslash       (less_ctx->pub.__opt_use_backslash)
#define no_hist_dups            (less_ctx->pub.__no_hist_dups)
#define mousecap                (less_ctx->pub.__mousecap)
#define wheel_lines             (less_ctx->pub.__wheel_lines)
#define perma_marks             (less_ctx->pub.__perma_marks)
#define linenum_width           (less_ctx->pub.__linenum_width)
#define status_col_width        (less_ctx->pub.__status_col_width)
#define incr_search             (less_ctx->pub.__incr_search)
#define use_color               (less_ctx->pub.__use_color)
#define want_filesize           (less_ctx->pub.__want_filesize)
#define status_line             (less_ctx->pub.__status_line)
#define nonum_headers           (less_ctx->pub.__nonum_headers)
#define redraw_on_quit          (less_ctx->pub.__redraw_on_quit)

    struct loption __option[] =
{
    { 'a', &a_optname,
        TRIPLE, OPT_ONPLUS, &how_search, NULL,
        {
            "Search includes displayed screen",
            "Search skips displayed screen",
            "Search includes all of displayed screen"
        }
    },

    { 'b', &b_optname,
        NUMBER|INIT_HANDLER, 64, &bufspace, opt_b, 
        {
            "Max buffer space per file (K): ",
            "Max buffer space per file: %dK",
            NULL
        }
    },
    { 'B', &B__optname,
        BOOL, OPT_ON, &autobuf, NULL,
        {
            "Don't automatically allocate buffers",
            "Automatically allocate buffers when needed",
            NULL
        }
    },
    { 'c', &c_optname,
        TRIPLE, OPT_OFF, &top_scroll, NULL,
        {
            "Repaint by scrolling from bottom of screen",
            "Repaint by painting from top of screen",
            "Repaint by painting from top of screen"
        }
    },
    { 'd', &d_optname,
        BOOL|NO_TOGGLE, OPT_OFF, &know_dumb, NULL,
        {
            "Assume intelligent terminal",
            "Assume dumb terminal",
            NULL
        }
    },
    { 'D', &D__optname,
        STRING|REPAINT|NO_QUERY, 0, NULL, opt_D,
        {
            "color desc: ", 
            NULL,
            NULL
        }
    },
    { 'e', &e_optname,
        TRIPLE, OPT_OFF, &quit_at_eof, NULL,
        {
            "Don't quit at end-of-file",
            "Quit at end-of-file",
            "Quit immediately at end-of-file"
        }
    },
    { 'f', &f_optname,
        BOOL, OPT_OFF, &force_open, NULL,
        {
            "Open only regular files",
            "Open even non-regular files",
            NULL
        }
    },
    { 'F', &F__optname,
        BOOL, OPT_OFF, &quit_if_one_screen, NULL,
        {
            "Don't quit if end-of-file on first screen",
            "Quit if end-of-file on first screen",
            NULL
        }
    },
#if HILITE_SEARCH
    { 'g', &g_optname,
        TRIPLE|HL_REPAINT, OPT_ONPLUS, &hilite_search, NULL,
        {
            "Don't highlight search matches",
            "Highlight matches for previous search only",
            "Highlight all matches for previous search pattern",
        }
    },
#endif
    { 'h', &h_optname,
        NUMBER, -1, &back_scroll, NULL,
        {
            "Backwards scroll limit: ",
            "Backwards scroll limit is %d lines",
            NULL
        }
    },
    { 'i', &i_optname,
        TRIPLE|HL_REPAINT, OPT_OFF, &caseless, opt_i,
        {
            "Case is significant in searches",
            "Ignore case in searches",
            "Ignore case in searches and in patterns"
        }
    },
    { 'j', &j_optname,
        STRING, 0, NULL, opt_j,
        {
            "Target line: ",
            "0123456789.-",
            NULL
        }
    },
    { 'J', &J__optname,
        BOOL|REPAINT, OPT_OFF, &status_col, NULL,
        {
            "Don't display a status column",
            "Display a status column",
            NULL
        }
    },
#if USERFILE
    { 'k', &k_optname,
        STRING|NO_TOGGLE|NO_QUERY, 0, NULL, opt_k,
        { NULL, NULL, NULL }
    },
#if HAVE_LESSKEYSRC 
    { OLETTER_NONE, &ks_optname,
        STRING|NO_TOGGLE|NO_QUERY, 0, NULL, opt_ks,
        { NULL, NULL, NULL }
    },
#endif /* HAVE_LESSKEYSRC */
#endif
    { 'K', &K__optname,
        BOOL, OPT_OFF, &quit_on_intr, NULL,
        {
            "Interrupt (ctrl-C) returns to prompt",
            "Interrupt (ctrl-C) exits less",
            NULL
        }
    },
    { 'L', &L__optname,
        BOOL, OPT_ON, &use_lessopen, NULL,
        {
            "Don't use the LESSOPEN filter",
            "Use the LESSOPEN filter",
            NULL
        }
    },
    { 'm', &m_optname,
        TRIPLE, OPT_OFF, &pr_type, NULL,
        {
            "Short prompt",
            "Medium prompt",
            "Long prompt"
        }
    },
    { 'n', &n_optname,
        TRIPLE|REPAINT, OPT_ON, &linenums, NULL,
        {
            "Don't use line numbers",
            "Use line numbers",
            "Constantly display line numbers"
        }
    },
#if LOGFILE
    { 'o', &o_optname,
        STRING, 0, NULL, opt_o,
        { "log file: ", NULL, NULL }
    },
    { 'O', &O__optname,
        STRING, 0, NULL, opt__O,
        { "Log file: ", NULL, NULL }
    },
#endif
    { 'p', &p_optname,
        STRING|NO_TOGGLE|NO_QUERY, 0, NULL, opt_p,
        { NULL, NULL, NULL }
    },
    { 'P', &P__optname,
        STRING, 0, NULL, opt__P,
        { "prompt: ", NULL, NULL }
    },
    { 'q', &q_optname,
        TRIPLE, OPT_OFF, &quiet, NULL,
        {
            "Ring the bell for errors AND at eof/bof",
            "Ring the bell for errors but not at eof/bof",
            "Never ring the bell"
        }
    },
    { 'r', &r_optname,
        TRIPLE|REPAINT, OPT_OFF, &ctldisp, NULL,
        {
            "Display control characters as ^X",
            "Display control characters directly (not recommended)",
            "Display ANSI sequences directly, other control characters as ^X"
        }
    },
    { 's', &s_optname,
        BOOL|REPAINT, OPT_OFF, &squeeze, NULL,
        {
            "Display all blank lines",
            "Squeeze multiple blank lines",
            NULL
        }
    },
    { 'S', &S__optname,
        BOOL|REPAINT, OPT_OFF, &chopline, NULL,
        {
            "Fold long lines",
            "Chop long lines",
            NULL
        }
    },
#if TAGS
    { 't', &t_optname,
        STRING|NO_QUERY, 0, NULL, opt_t,
        { "tag: ", NULL, NULL }
    },
    { 'T', &T__optname,
        STRING, 0, NULL, opt__T,
        { "tags file: ", NULL, NULL }
    },
#endif
    { 'u', &u_optname,
        TRIPLE|REPAINT, OPT_OFF, &bs_mode, NULL,
        {
            "Display underlined text in underline mode",
            "Backspaces cause overstrike",
            "Print backspace as ^H"
        }
    },
    { 'V', &V__optname,
        NOVAR, 0, NULL, opt__V,
        { NULL, NULL, NULL }
    },
    { 'w', &w_optname,
        TRIPLE|REPAINT, OPT_OFF, &show_attn, NULL,
        {
            "Don't highlight first unread line",
            "Highlight first unread line after forward-screen",
            "Highlight first unread line after any forward movement",
        }
    },
    { 'x', &x_optname,
        STRING|REPAINT, 0, NULL, opt_x,
        {
            "Tab stops: ",
            "0123456789,",
            NULL
        }
    },
    { 'X', &X__optname,
        BOOL|NO_TOGGLE, OPT_OFF, &no_init, NULL,
        {
            "Send init/deinit strings to terminal",
            "Don't use init/deinit strings",
            NULL
        }
    },
    { 'y', &y_optname,
        NUMBER, -1, &forw_scroll, NULL,
        {
            "Forward scroll limit: ",
            "Forward scroll limit is %d lines",
            NULL
        }
    },
    { 'z', &z_optname,
        NUMBER, -1, &swindow, NULL,
        {
            "Scroll window size: ",
            "Scroll window size is %d lines",
            NULL
        }
    },
    { '"', &quote_optname,
        STRING, 0, NULL, opt_quote,
        { "quotes: ", NULL, NULL }
    },
    { '~', &tilde_optname,
        BOOL|REPAINT, OPT_ON, &twiddle, NULL,
        {
            "Don't show tildes after end of file",
            "Show tildes after end of file",
            NULL
        }
    },
    { '?', &query_optname,
        NOVAR, 0, NULL, opt_query,
        { NULL, NULL, NULL }
    },
    { '#', &pound_optname,
        STRING, 0, NULL, opt_shift,
        {
            "Horizontal shift: ",
            "0123456789.",
            NULL
        }
    },
    { OLETTER_NONE, &keypad_optname,
        BOOL|NO_TOGGLE, OPT_OFF, &no_keypad, NULL,
        {
            "Use keypad mode",
            "Don't use keypad mode",
            NULL
        }
    },
    { OLETTER_NONE, &oldbot_optname,
        BOOL, OPT_OFF, &oldbot, NULL,
        {
            "Use new bottom of screen behavior",
            "Use old bottom of screen behavior",
            NULL
        }
    },
    { OLETTER_NONE, &follow_optname,
        BOOL, FOLLOW_DESC, &follow_mode, NULL,
        {
            "F command follows file descriptor",
            "F command follows file name",
            NULL
        }
    },
    { OLETTER_NONE, &use_backslash_optname,
        BOOL, OPT_OFF, &opt_use_backslash, NULL,
        {
            "Use backslash escaping in command line parameters",
            "Don't use backslash escaping in command line parameters",
            NULL
        }
    },
    { OLETTER_NONE, &rscroll_optname,
        STRING|REPAINT|INIT_HANDLER, 0, NULL, opt_rscroll,
        { "right scroll character: ", NULL, NULL }
    },
    { OLETTER_NONE, &nohistdups_optname,
        BOOL, OPT_OFF, &no_hist_dups, NULL,
        {
            "Allow duplicates in history list",
            "Remove duplicates from history list",
            NULL
        }
    },
    { OLETTER_NONE, &mousecap_optname,
        TRIPLE, OPT_OFF, &mousecap, opt_mousecap,
        {
            "Ignore mouse input",
            "Use the mouse for scrolling",
            "Use the mouse for scrolling (reverse)"
        }
    },
    { OLETTER_NONE, &wheel_lines_optname,
        NUMBER|INIT_HANDLER, 0, &wheel_lines, opt_wheel_lines,
        {
            "Lines to scroll on mouse wheel: ",
            "Scroll %d line(s) on mouse wheel",
            NULL
        }
    },
    { OLETTER_NONE, &perma_marks_optname,
        BOOL, OPT_OFF, &perma_marks, NULL,
        {
            "Don't save marks in history file",
            "Save marks in history file",
            NULL
        }
    },
    { OLETTER_NONE, &linenum_width_optname,
        NUMBER|REPAINT, MIN_LINENUM_WIDTH, &linenum_width, opt_linenum_width,
        {
            "Line number width: ",
            "Line number width is %d chars",
            NULL
        }
    },
    { OLETTER_NONE, &status_col_width_optname,
        NUMBER|REPAINT, 2, &status_col_width, opt_status_col_width,
        {
            "Status column width: ",
            "Status column width is %d chars",
            NULL
        }
    },
    { OLETTER_NONE, &incr_search_optname,
        BOOL, OPT_OFF, &incr_search, NULL,
        {
            "Incremental search is off",
            "Incremental search is on",
            NULL
        }
    },
    { OLETTER_NONE, &use_color_optname,
        BOOL|REPAINT, OPT_OFF, &use_color, NULL,
        {
            "Don't use color",
            "Use color",
            NULL
        }
    },
    { OLETTER_NONE, &want_filesize_optname,
        BOOL|REPAINT, OPT_OFF, &want_filesize, opt_filesize,
        {
            "Don't get size of each file",
            "Get size of each file",
            NULL
        }
    },
    { OLETTER_NONE, &status_line_optname,
        BOOL|REPAINT, OPT_OFF, &status_line, NULL,
        {
            "Don't color each line with its status column color",
            "Color each line with its status column color",
            NULL
        }
    },
    { OLETTER_NONE, &header_optname,
        STRING|REPAINT, 0, NULL, opt_header,
        {
            "Header lines: ",
            NULL,
            NULL
        }
    },
    { OLETTER_NONE, &nonum_headers_optname,
        BOOL|REPAINT, 0, &nonum_headers, NULL,
        {
            "Number header lines",
            "Don't number header lines",
            NULL
        }
    },
    { OLETTER_NONE, &redraw_on_quit_optname,
        BOOL, OPT_OFF, &redraw_on_quit, NULL,
        {
            "Don't redraw screen when quitting",
            "Redraw last screen when quitting",
            NULL
        }
    },
    { OLETTER_NONE, &search_type_optname,
        STRING, 0, NULL, opt_search_type,
        {
            "Search options: ",
            NULL,
            NULL
        }
    },
#if LESSTEST
    { OLETTER_NONE, &ttyin_name_optname,
        STRING|NO_TOGGLE, 0, NULL, opt_ttyin_name,
        {
            NULL,
            NULL,
            NULL
        }
    },
    { OLETTER_NONE, &rstat_optname,
        STRING|NO_TOGGLE, 0, NULL, opt_rstat,
        {
            NULL,
            NULL,
            NULL
        }
    },
#endif /*LESSTEST*/
    { '\0', NULL, NOVAR, 0, NULL, NULL, { NULL, NULL, NULL } }
};
    if (dimof(__option) > dimof(less_ctx->opttbl.__option)) {
        fprintf(stderr, "not enough space in less_ctx->opttbl.__option, need %d\n", dimof(__option));
        VSF_LINUX_ASSERT(false);
        return -1;
    }
    memcmp(less_ctx->opttbl.__option, __option, sizeof(__option));
    return err;
}

void * __less_ctx(void)
{
    return &((struct less_lib_ctx_t *)vsf_linux_library_ctx(__less_lib_idx))->__less_ctx;
}

#endif        // __VSF__

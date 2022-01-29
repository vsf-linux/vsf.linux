struct optname
{
        char *oname;            /* Long (GNU-style) option name */
        struct optname *onext;  /* List of synonymous option names */
};

#define OPTNAME_MAX     32      /* Max length of long option name */

struct loption
{
        char oletter;           /* The controlling letter (a-z) */
        struct optname *onames; /* Long (GNU-style) option name */
        int otype;              /* Type of the option */
        int odefault;           /* Default value */
        int *ovar;              /* Pointer to the associated variable */
        void (*ofunc) LESSPARAMS ((int, char*)); /* Pointer to special handling function */
        char *odesc[3];         /* Description of each value */
};

struct charset {
    char *name;
    int *p_flag;
    char *desc;
};

struct mlist
{
    struct mlist *next;
    struct mlist *prev;
    struct mlist *curr_mp;
    char *string;
    int modified;
};

struct tablelist
{
    struct tablelist *t_next;
    char *t_start;
    char *t_end;
};

struct ifile {
    struct ifile *h_next;           /* Links for command line list */
    struct ifile *h_prev;
    char *h_filename;               /* Name of the file */
    char *h_rfilename;              /* Canonical name of the file */
    void *h_filestate;              /* File state (used in ch.c) */
    int h_index;                    /* Index within command line list */
    int h_hold;                     /* Hold count */
    char h_opened;                  /* Has this ifile been opened? */
    struct scrpos h_scrpos;         /* Saved position within the file */
    void *h_altpipe;                /* Alt pipe */
    char *h_altfilename;            /* Alt filename */
};

struct ungot {
    struct ungot *ug_next;
    LWCHAR ug_char;
};

#define NUM_LAST_ANSIS 3
#define MAX_PFX_WIDTH (MAX_LINENUM_WIDTH + MAX_STATUSCOL_WIDTH + 1)
struct s__linebuf {
    char *buf;    /* Buffer which holds the current output line */
    int *attr;   /* Parallel to buf, to hold attributes */
    int print;    /* Index in buf of first printable char */
    int end;      /* Number of chars in buf */
    char pfx[MAX_PFX_WIDTH]; /* Holds status column and line number */
    int pfx_attr[MAX_PFX_WIDTH];
    int pfx_end;  /* Number of chars in pfx */
};

struct linenum_info
{
    struct linenum_info *next;      /* Link to next in the list */
    struct linenum_info *prev;      /* Line to previous in the list */
    POSITION pos;                   /* File position */
    POSITION gap;                   /* Gap between prev and next */
    LINENUM line;                   /* Line number */
};

struct mark 
{
    /*
     * Normally m_ifile != IFILE_NULL and m_filename == NULL.
     * For restored marks we set m_filename instead of m_ifile
     * because we don't want to create an ifile until the 
     * user explicitly requests the file (by name or mark).
     */
    char m_letter;           /* Associated character */
    IFILE m_ifile;           /* Input file being marked */
    char *m_filename;        /* Name of the input file */
    struct scrpos m_scrpos;  /* Position of the mark */
};

struct pattern_info {
    PATTERN_TYPE compiled;
    char* text;
    int search_type;
    struct pattern_info *next;
};

struct taglist {
    struct tag *tl_first;
    struct tag *tl_last;
};
struct tag {
    struct tag *next, *prev; /* List links */
    char *tag_file;         /* Source file containing the tag */
    LINENUM tag_linenum;    /* Appropriate line number in source file */
    char *tag_pattern;      /* Pattern used to find the tag */
    char tag_endline;       /* True if the pattern includes '$' */
};

struct hilite
{
    POSITION hl_startpos;
    POSITION hl_endpos;
};
struct hilite_node
{
    struct hilite_node *parent;
    struct hilite_node *left;
    struct hilite_node *right;
    struct hilite_node *prev;
    struct hilite_node *next;
    int red;
    struct hilite r;
};
struct hilite_storage
{
    int capacity;
    int used;
    struct hilite_storage *next;
    struct hilite_node *nodes;
};
struct hilite_tree
{
    struct hilite_storage *first;
    struct hilite_storage *current;
    struct hilite_node *root;
    struct hilite_node *lookaside;
};

struct env { struct env *next; char *name; char *value; };

struct less_ctx_t {
    struct {
        // ch
        int __ignore_eoi;

        // charset
        int __utf_mode;
        int __binattr;              // = AT_STANDOUT|AT_COLOR_BIN;

        // cmdbuf
        int __updown_match;         // = -1;
#if SPACES_IN_FILENAMES
        char __openquote;           // = '"';
        char __closequote;          // = '"';
#endif
        void *__ml_search;          // = (void *)1; = (void *) &mlist_search;
        void *__ml_examine;         // = (void *)2; = (void *) &mlist_examine;
#if SHELL_ESCAPE || PIPEC
        void *__ml_shell;           // = (void *)3; = (void *) &mlist_shell;
#endif

        // edit
        int __fd0;

        // forwback
        int __screen_trashed;
        int __squished;
        int __no_back_scroll;
        int __forw_prompt;
        int __first_time;           // = 1;

        // line
        int __size_linebuf;
        int __hshift;
        int __tabstops[TABSTOP_MAX];
        int __ntabstops;            // = 1;
        int __tabdefault;           // = 8;
        POSITION __highest_hilite;

        // main
        char *__every_first_cmd;
        int __new_file;
        int __is_tty;
        IFILE __curr_ifile;
        IFILE __old_ifile;
        struct scrpos __initial_scrpos;
        POSITION __start_attnpos;   // = NULL_POSITION;
        POSITION __end_attnpos;     // = NULL_POSITION;
        int __wscroll;
        char *__progname;
        int __quitting;
        int __secure;
        int __dohelp;
#if LOGFILE
        int __logfile;              // = -1;
        int __force_logfile;
        char *__namelogfile;
#endif
#if EDITOR
        char *__editor;
        char *__editproto;
#endif
        int __one_screen;

        // mark
        int __marks_modified;

        // optfunc
        char *__tagoption;

        // opttbl
        int __quiet;
        int __how_search;
        int __top_scroll;
        int __pr_type;
        int __bs_mode;
        int __know_dumb;
        int __quit_at_eof;
        int __quit_if_one_screen;
        int __squeeze;
        int __tabstop;
        int __back_scroll;
        int __forw_scroll;
        int __caseless;
        int __linenums;
        int __autobuf;
        int __bufspace;
        int __ctldisp;
        int __force_open;
        int __swindow;
        int __jump_sline;
        long __jump_sline_fraction;   // = -1;
        long __shift_count_fraction;  // = -1;
        int __chopline;
        int __no_init;
        int __no_keypad;
        int __twiddle;
        int __show_attn;
        int __shift_count;
        int __status_col;
        int __use_lessopen;
        int __quit_on_intr;
        int __follow_mode;
        int __oldbot;
        int __opt_use_backslash;
        char __rscroll_char;
        int __rscroll_attr;
        int __no_hist_dups;
        int __mousecap;
        int __wheel_lines;
        int __perma_marks;
        int __linenum_width;
        int __status_col_width;
        int __incr_search;
        int __use_color;
        int __want_filesize;
        int __status_line;
        int __header_lines;
        int __header_cols;
        int __nonum_headers;
        int __redraw_on_quit;
        int __def_search_type;
#if HILITE_SEARCH
        int __hilite_search;
#endif
        int __less_is_more;

        // os
        int __reading;
        int __consecutive_nulls;

        // output
        int __errmsgs;
        int __need_clr;
        int __final_attr;
        int __at_prompt;

        // prompt
        char *__prproto[3];
        char constant *__eqproto;   // = e_proto;
        char constant *__hproto;    // = h_proto;
        char constant *__wproto;    // = w_proto;

        // screen
        int __auto_wrap;
        int __ignaw;
        int __erase_char;
        int __erase2_char;
        int __kill_char;
        int __werase_char;
        int __sc_width, __sc_height;
        int __bo_s_width, __bo_e_width;
        int __ul_s_width, __ul_e_width;
        int __so_s_width, __so_e_width;
        int __bl_s_width, __bl_e_width;
        int __above_mem, __below_mem;
        int __can_goto_line;
        int __clear_bg;
        int __missing_cap;
        char *__kent;
        int __term_init_done;

        // search
        int __is_caseless;

        // signal
        int __sigs;

        // tags
#if TAGS
        char *__tags;               //  = ztags;
#endif

        // ttyin
        int __tty;

        // option
        int __plusoption;
    } pub;

    struct {
        struct filestate *__thisfile;
        int __ch_ungotchar;         // = -1;
        int __maxbufs;              // = -1;
#if LOGFILE
        struct {
            int __tried;
        } end_logfile;
#endif
    } ch;

    struct {
        char __chardef[256];
        char *__binfmt;
        char *__utfbinfmt;
        struct charset __charsets[21];

        struct {
            char __buf[MAX_PRCHAR_LEN+1];
        } prchar;
        struct {
            char __buf[MAX_PRCHAR_LEN+1];
        } prutfchar;
    } charset;

    struct {
        char __cmdbuf[CMDBUF_SIZE];
        int __cmd_col;
        int __prompt_col;
        char *__cp;
        int __cmd_offset;
        int __literal;

#if TAB_COMPLETE_FILENAME
        int __in_completion;
        char *__tk_text;
        char *__tk_original;
        char *__tk_ipoint;
        char *__tk_trial;
        struct textlist __tk_tlist;
#endif

        int __curr_cmdflags;
        char __cmd_mbc_buf[MAX_UTF_CHAR_LEN];
        int __cmd_mbc_buf_len;
        int __cmd_mbc_buf_index;

#if CMD_HISTORY
        struct mlist __mlist_search;    // = { &mlist_search,  &mlist_search,  &mlist_search,  NULL, 0 };
        struct mlist __mlist_examine;   // = { &mlist_examine, &mlist_examine, &mlist_examine, NULL, 0 };
#endif

#if SHELL_ESCAPE || PIPEC
        struct mlist __mlist_shell;     // = { &mlist_shell,   &mlist_shell,   &mlist_shell,   NULL, 0 };
#endif
        struct mlist *__curr_mlist;
    } __cmdbuf;

    struct {
#if SHELL_ESCAPE
        char *__shellcmd;
#endif
        int __mca;
        int __search_type;
        LINENUM __number;
        long __fraction;
        struct loption *__curropt;
        int __opt_lower;
        int __optflag;
        int __optgetname;
        POSITION __bottompos;
        int __save_hshift;
        int __save_bs_mode;
#if PIPEC
        char __pipec;
#endif
        struct ungot* __ungot;
    } command;

    struct {
        struct tablelist *__list_fcmd_tables;
        struct tablelist *__list_ecmd_tables;
        struct tablelist *__list_var_tables;
        struct tablelist *__list_sysvar_tables;

        unsigned char __cmdtable[501];
        unsigned char __edittable[216];

#if USERFILE && HAVE_LESSKEYSRC
        struct {
            struct lesskey_tables __tables;
        } lesskey_src;
#endif
    } decode;

    struct {
        struct {
            char *__mchars;
        } metachars;
    } filename;

    struct {
        struct {
            time_type __last_eof_bell;
        } eof_bell;
    } forwback;

    struct {
        struct ifile __anchor;      // = { &anchor, &anchor, NULL, NULL, NULL, 0, 0, '\0', { NULL_POSITION, 0 } };
        int __ifiles;
    } ifile;

    struct {
        int __linenum;
        int __errors;
        int __less_version;
        char *__lesskey_file;

        struct {
            char __buf[8];
            char __tstr_control_k[7]; // = { SK_SPECIAL_KEY, SK_CONTROL_K, 6, 1, 1, 1, '\0' };
        } tstr;
    } lesskey_parser;

    struct {
        char __mbc_buf[MAX_UTF_CHAR_LEN];
        int __mbc_buf_len;
        int __mbc_buf_index;
        POSITION __mbc_pos;

        struct s__linebuf __linebuf;
        struct xbuffer __shifted_ansi;

        struct xbuffer __last_ansi;
        struct xbuffer __last_ansis[NUM_LAST_ANSIS];
        int __curr_last_ansi;

        struct ansi_state *__line_ansi;
        int __ansi_in_line;
        int __hlink_in_line;
        int __line_mark_attr;
        int __cshift;

        int __end_column;
        int __right_curr;
        int __right_column;
        int __overstrike;
        int __last_overstrike;
        int __is_null_line;
        LWCHAR __pendc;
        POSITION __pendpos;
        char *__end_ansi_chars;
        char *__mid_ansi_chars;
        int __in_hilite;

        struct {
            char __hlink_prefix[5]; // = ESCS "]8;";
        } ansi_step;
    } line;

    struct {
        struct linenum_info __anchor;
        struct linenum_info *__freelist;
        struct linenum_info __pool[200];
        struct linenum_info *__spare;

        int __loopcount;
#if HAVE_TIME
        time_type __startime;
#endif
    } linenum;

    struct {
#ifdef WIN32
        char __consoleTitle[256];
#endif

        struct {
            int __save_status;
        } quit;
    } __main;

    struct {
        struct mark __marks[2 * 26 + 2];

        struct {
            struct mark __sm;
        } getmark;
    } mark;

    struct {
        struct loption *__pendopt;

        struct {
            char __buf[MAX_PRCHAR_LEN+2];
        } propt;
    } option;

    struct {
        jmp_buf __read_label;

        struct {
            char __buf[INT_STRLEN_BOUND(int)+12];
        } strerror;
    } os;

    struct {
        char __obuf[OUTBUF_SIZE];
        char *__ob;                 // = obuf;
        int __outfd;                // = 2;
    } output;

    struct {
        POSITION *__table;
        int __table_size;
    } position;

    struct {
        char __message[PROMPT_SIZE];
        char *__mp;
    } prompt;

    struct {
        char *__regparse;
        int __regnpar;
        char __regdummy;
        char *__regcode;
        long __regsize;

        char *__reginput;
        char *__regbol;
        char **__regstartp;
        char **__regendp;
    } regexp;

    struct {
#if !MSDOS_COMPILER
        char*__sc_pad,
            *__sc_home,
            *__sc_addline,
            *__sc_lower_left,
            *__sc_return,
            *__sc_move,
            *__sc_clear,
            *__sc_eol_clear,
            *__sc_eos_clear,
            *__sc_s_in,
            *__sc_s_out,
            *__sc_u_in,
            *__sc_u_out,
            *__sc_b_in,
            *__sc_b_out,
            *__sc_bl_in,
            *__sc_bl_out,
            *__sc_visual_bell,
            *__sc_backspace,
            *__sc_s_keypad,
            *__sc_e_keypad,
            *__sc_s_mousecap,
            *__sc_e_mousecap,
            *__sc_init,
            *__sc_deinit;
        int __attrcolor;            // = -1;
        int __hardcopy;
        int __costcount;
#endif
        int __init_done;
        int __attrmode;
        int __termcap_debug;        // = -1;

        struct {
            int __curr_on;
        } raw_mode;
        struct {
            struct env *__envs;
        } ltget_env;
        struct {
            char __tbuf[40];
        } special_key_str;
        struct {
            char __termbuf[TERMBUF_SIZE];
            char __sbuf[TERMSBUF_SIZE];
        } get_term;
    } screen;

    struct {
        int __hide_hilite;
        POSITION __prep_startpos;
        POSITION __prep_endpos;
        struct hilite_tree __hilite_anchor;
        struct hilite_tree __filter_anchor;
        struct pattern_info *__filter_infos;
        struct pattern_info __search_info;
        int __is_ucase_pattern;
    } search;

#if TAGS
    struct {
        struct taglist __taglist;   // = { TAG_END, TAG_END };
        struct tag *__curtag;
        int __total;
        int __curseq;
        int __circular;
    } tags;
#endif

    struct {
        struct loption __option[55];
    } opttbl;
};

extern int vsf_linux_less_init(void);

extern void * __less_ctx(void);
#define less_ctx            ((struct less_ctx_t *)__less_ctx())

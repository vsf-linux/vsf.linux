#include <termios.h>
#include <pthread.h>
#include <regex.h>

#include "strbuf.h"
#include "hashmap.h"
#include "string-list.h"
#include "trace.h"

#include "ewah/ewok.h"
#include "refs/refs-internal.h"
#include "trace2/tr2_sysenv.h"
#include "trace2/tr2_dst.h"
#include "trace2/tr2_tgt.h"

enum mmap_strategy {
    /*
     * Don't use mmap() at all for reading `packed-refs`.
     */
    MMAP_NONE,

    /*
     * Can use mmap() for reading `packed-refs`, but the file must
     * not remain mmapped. This is the usual option on Windows,
     * where you cannot rename a new version of a file onto a file
     * that is currently mmapped.
     */
    MMAP_TEMPORARY,

    /*
     * It is OK to leave the `packed-refs` file mmapped while
     * arbitrary other code is running.
     */
    MMAP_OK
};

struct tr2_sysenv_entry {
    const char *env_var_name;
    const char *git_config_name;

    char *value;
    unsigned int getenv_called : 1;
};

struct git_ctx_t {
    // compat
    struct {
        void (*__obstack_alloc_failed_handler) (void);  // = print_and_abort;
    } obstack;
    struct {
#if defined(HAVE_DEV_TTY) || defined(GIT_WINDOWS_NATIVE)
#   ifdef HAVE_DEV_TTY
        int __term_fd;                  // = -1;
        struct termios __old_term;
#   elif defined(GIT_WINDOWS_NATIVE)
#   endif
        struct {
            struct strbuf __buf;        // = STRBUF_INIT;
        } git_terminal_prompt;
        struct {
            struct hashmap __sequences;
	        int __initialized;
        } is_known_escape_sequence;
#endif
        struct {
            int __warning_displayed;
        } read_key_without_echo;
    } terminal;
    struct {
        reg_syntax_t __re_syntax_options;
#ifdef RE_ENABLE_I18N
#   if !defined(__GNUC__) || __GNUC__ < 3
        bitset_t __utf8_sb_map;
        struct {
            short __utf8_sb_map_inited;
        } init_dfa;
#   endif
#endif
#if defined _REGEX_RE_COMP || defined _LIBC
        struct re_pattern_buffer __re_comp_buf;
#endif
    } regex;

    // ewah
    struct {
#define BITMAP_POOL_MAX 16
        struct ewah_bitmap *__bitmap_pool[BITMAP_POOL_MAX];
        size_t __bitmap_pool_size;
    } ewah_bitmap;

    // negotiator
    struct {
        int __marked;
    } negotiator_default;
    struct {
        int __marked;
    } negotiator_skipping;

    // refs
    struct {
        struct trace_key __trace_refs;      // = TRACE_KEY_INIT(REFS);
    } refs_debug;
    struct {
        struct ref_iterator *__current_ref_iter;
    } refs_iterator;
    struct {
        enum mmap_strategy __mmap_strategy; // = MMAP_NONE/MMAP_TEMPORARY/MMAP_OK;
        struct {
            int __timeout_configured;
            int __timeout_value;            // = 1000;
        } packed_refs_lock;
    } refs_packed_backend;

    struct {
        struct ref_storage_be *__refs_backends; // = &refs_be_files;
        struct {
            char *__ret;
        } git_default_branch_name;
        struct {
            int __configured;
            int __timeout_ms;           // = 100;
        } get_files_ref_lock_timeout_ms;
        struct {
            char **__scanf_fmts;
            int __nr_rules;
        } refs_shorten_unambiguous_ref;
        struct {
            int __ref_paranoia;  // = -1;
        }refs_ref_iterator_begin;
        struct {
            struct strbuf __sb_refname;         // = STRBUF_INIT;
        } refs_resolve_ref_unsafe;
        struct string_list *__hide_refs;
        struct hashmap __submodule_ref_stores;
        struct hashmap __worktree_ref_stores;
    } refs;

    // trace2
    struct {
        struct strbuf **__tr2_cfg_patterns;
        int __tr2_cfg_count_patterns;
        int __tr2_cfg_loaded;

        struct strbuf **__tr2_cfg_env_vars;
        int __tr2_cfg_env_vars_count;
        int __tr2_cfg_env_vars_loaded;
    } tr2_cfg;
    struct {
        struct strbuf __tr2cmdname_hierarchy;   // = STRBUF_INIT;
    } tr2_cmd_name;
    struct {
        int __tr2env_max_files;
        struct {
            int __tr2env_dst_debug;             // = -1;
        } tr2_dst_want_warning;
    } tr2_dst;
    struct {
        struct strbuf __tr2sid_buf;             // = STRBUF_INIT;
        int __tr2sid_nr_git_parents;
    } tr2_sid;
    struct {
        struct tr2_sysenv_entry __tr2_sysenv_settings[10];
    } tr2_sysenv;
    struct {
        struct tr2_dst __tr2dst_event;          // = { TR2_SYSENV_EVENT, 0, 0, 0, 0 };
        int __tr2env_event_max_nesting_levels;  // = 2;
        int __tr2env_event_be_brief;
        struct tr2_tgt __tr2_tgt_event;
    } __tr2_tgt_event;
    struct {
        struct tr2_dst __tr2dst_normal;         // = { TR2_SYSENV_NORMAL, 0, 0, 0, 0 };
        int __tr2env_normal_be_brief;
        struct tr2_tgt __tr2_tgt_normal;
    } __tr2_tgt_normal;
    struct {
        struct tr2_dst __tr2dst_perf;           // = { TR2_SYSENV_PERF, 0, 0, 0, 0 };
        int __tr2env_perf_be_brief;
        struct tr2_tgt __tr2_tgt_perf;
    } __tr2_tgt_perf;
    struct {
        struct tr2tls_thread_ctx *__tr2tls_thread_main;
        uint64_t __tr2tls_us_start_process;
        pthread_mutex_t __tr2tls_mutex;
        pthread_key_t __tr2tls_key;
        int __tr2_next_thread_id;
    } tr2_tls;
    struct {
        int __trace2_enabled;
        int __tr2_next_child_id;
        int __tr2_next_exec_id;
        int __tr2_next_repo_id;                 // = 1;
        struct tr2_tgt *__tr2_tgt_builtins[4];
        int __tr2main_exit_code;
    } trace2;

    // builtin
    // src
};

void * __git_ctx(void);
#define git_ctx                 ((struct git_ctx_t *)__git_ctx())

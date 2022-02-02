#include <termios.h>
#include <pthread.h>
#include <regex.h>

#include "strbuf.h"
#include "hashmap.h"
#include "string-list.h"
#include "trace.h"
#include "color.h"
#include "parse-options.h"
#include "entry.h"
#include "list-objects-filter-options.h"
#include "strvec.h"
#include "wt-status.h"
#include "lockfile.h"
#include "commit-graph.h"
#include "config.h"
#include "credential.h"
#include "commit-slab.h"
#include "commit.h"
#include "decorate.h"
#include "refspec.h"
#include "revision.h"
#include "pack.h"

#include "ewah/ewok.h"
#include "refs/refs-internal.h"
#include "trace2/tr2_sysenv.h"
#include "trace2/tr2_dst.h"
#include "trace2/tr2_tgt.h"

// refs_packed_backend
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

// tr2_sysenv
struct tr2_sysenv_entry {
    const char *env_var_name;
    const char *git_config_name;

    char *value;
    unsigned int getenv_called : 1;
};

// builtin_blame
struct color_field {
	timestamp_t hop;
	char col[COLOR_MAXLEN];
};

// builtin_credential_cache_daemon
struct credential_cache_entry {
	struct credential item;
	timestamp_t expiration;
};

// builtin_describe
define_commit_slab(commit_names, struct commit_name *);

// builtin_fast_import
struct last_object {
	struct strbuf data;
	off_t offset;
	unsigned int depth;
	unsigned no_swap : 1;
};

typedef enum {
	WHENSPEC_RAW = 1,
	WHENSPEC_RAW_PERMISSIVE,
	WHENSPEC_RFC2822,
	WHENSPEC_NOW
} whenspec_type;

struct recent_command {
	struct recent_command *prev;
	struct recent_command *next;
	char *buf;
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
        struct tr2_sysenv_entry __tr2_sysenv_settings[11];
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
    struct {
        int __patch_interactive, __add_interactive, __edit_interactive;
        int __take_worktree_changes;
        int __add_renormalize;
        int __pathspec_file_nul;
        int __include_sparse;
        const char *__pathspec_from_file;
        int __legacy_stash_p;
        int __verbose, __show_only, __ignored_too, __refresh_only;
        int __ignore_add_errors, __intent_to_add, __ignore_missing;
        int __warn_on_embedded_repo;            // = 1;
#define ADDREMOVE_DEFAULT 1
        int __addremove;                        // = ADDREMOVE_DEFAULT;
        int __addremove_explicit;               // = -1;
        char *__chmod_arg;
        struct {
            int __adviced_on_embedded_repo;
        } check_embedded_repo;
    } builtin_add;
    struct {
        struct {
            struct strbuf __sb;                 // = STRBUF_INIT;
        } msgnum;
    } builtin_am;
    struct {
        int __longest_file;
        int __longest_author;
        int __max_orig_digits;
        int __max_digits;
        int __max_score_digits;
        int __show_root;
        int __reverse;
        int __blank_boundary;
        int __incremental;
        int __xdl_opts;
        int __abbrev;                               // = -1;
        int __no_whole_file_rename;
        int __show_progress;
        char __repeated_meta_color[COLOR_MAXLEN];
        int __coloring_mode;
        struct string_list __ignore_revs_file_list; // = STRING_LIST_INIT_NODUP;
        int __mark_unblamable_lines;
        int __mark_ignored_lines;
        struct date_mode __blame_date_mode;         // = { DATE_ISO8601 };
        size_t __blame_date_width;
        struct string_list __mailmap;               // = STRING_LIST_INIT_NODUP;
        unsigned __blame_move_score;
        unsigned __blame_copy_score;
        struct color_field *__colorfield;
        int __colorfield_nr, __colorfield_alloc;
        struct {
            struct strbuf __time_buf;           // = STRBUF_INIT;
        } format_time;
    } builtin_blame;
    struct {
        const char *__head;
        struct object_id __head_oid;
        int __branch_use_color;                 // = -1;
        struct string_list __output;            // = STRING_LIST_INIT_DUP;
        unsigned int __colopts;
        struct {
            struct strbuf __buf;                // = STRBUF_INIT;
        } quote_literal_for_format;
        struct {
            struct ref_sorting *__sorting;;
        } cmd_branch;
    } builtin_branch;
    struct {
        const char *__force_path;
    } builtin_cat_file;
    struct {
        int __all_attrs;
        int __cached_attrs;
        int __stdin_paths;
        int __nul_term_line;
        struct option __check_attr_options[5];
    } builtin_check_attr;
    struct {
        int __quiet, __verbose, __stdin_paths, __show_non_matching, __no_index;
        int __nul_term_line;
    } builtin_check_ignore;
    struct {
        int __use_stdin;
    } builtin_check_mailmap;
    struct {
        char __cb_option;                       // = 'b';
    } builtin_checkout;
    struct {
#define CHECKOUT_ALL 4
        int __nul_term_line;
        int __checkout_stage;
        int __to_tempfile;
        char __topath[4][TEMPORARY_FILENAME_LENGTH + 1];
        struct checkout __state;                // = CHECKOUT_INIT;
    } builtin_checkout_index;
    struct {
        int __force;                            // = -1;
        int __interactive;
        struct string_list __del_list;          // = STRING_LIST_INIT_DUP;
        unsigned int __colopts;
        int __clean_use_color;                  // = -1;
    } builtin_clean;
    struct {
        int __option_no_checkout, __option_bare, __option_mirror;
        int __option_single_branch;             // = -1;
        int __option_local;                     // = -1;
        int __option_no_hardlinks, __option_shared;
        int __option_no_tags;
        int __option_shallow_submodules;
        int __option_reject_shallow;            // = -1;
        int __config_reject_shallow;            // = -1;
        int __deepen;
        char *__option_template, *__option_depth, *__option_since;
        char *__option_origin;
        char *__remote_name;
        char *__option_branch;
        struct string_list __option_not;        // = STRING_LIST_INIT_NODUP;
        const char *__real_git_dir;
        char *__option_upload_pack;             // = "git-upload-pack";
        int __option_verbosity;
        int __option_progress;                  // = -1;
        int __option_sparse_checkout;
        enum transport_family __family;
        struct string_list __option_config;             // = STRING_LIST_INIT_NODUP;
        struct string_list __option_required_reference; // = STRING_LIST_INIT_NODUP;
        struct string_list __option_optional_reference; // = STRING_LIST_INIT_NODUP;
        int __option_dissociate;
        int __max_jobs;                                 // = -1;
        struct string_list __option_recurse_submodules; // = STRING_LIST_INIT_NODUP;
        struct list_objects_filter_options __filter_options;
        struct string_list __server_options;            // = STRING_LIST_INIT_NODUP;
        int __option_remote_submodules;
        const char *__junk_work_tree;
        int __junk_work_tree_flags;
        const char *__junk_git_dir;
        int __junk_git_dir_flags;
        enum {
	        JUNK_LEAVE_NONE,
	        JUNK_LEAVE_REPO,
	        JUNK_LEAVE_ALL
        } __junk_mode;                          // = JUNK_LEAVE_NONE;
    } builtin_clone;
    struct {
        unsigned int __colopts;
    } builtin_column;
    struct {
        const char *__use_message_buffer;
        struct lock_file __index_lock;
        struct lock_file __false_lock;
        enum {
	        COMMIT_AS_IS = 1,
	        COMMIT_NORMAL,
	        COMMIT_PARTIAL
        } __commit_style;
        const char *__logfile, *__force_author;
        const char *__template_file;
        const char *__author_message, *__author_message_buffer;
        char *__edit_message, *__use_message;
        char *__fixup_message, *__fixup_commit, *__squash_message;
        const char *__fixup_prefix;
        int __all, __also, __interactive, __patch_interactive, __only, __amend, __signoff;
        int __edit_flag;                        // = -1;
        int __quiet, __verbose, __no_verify, __allow_empty, __dry_run, __renew_authorship;
        int __config_commit_verbose;            // = -1;
        int __no_post_rewrite, __allow_empty_message, __pathspec_file_nul;
        char *__untracked_files_arg, *__force_date, *__ignore_submodule_arg, *__ignored_arg;
        char *__sign_commit, *__pathspec_from_file;
        struct strvec __trailer_args;           // = STRVEC_INIT;
        enum commit_msg_cleanup_mode __cleanup_mode;
        const char *__cleanup_arg;
        enum commit_whence __whence;
        int __use_editor;                       // = 1;
        int __include_status;                   // = 1;
        int __have_option_m;
        struct strbuf __message;                // = STRBUF_INIT;
        enum wt_status_format __status_format;  // = STATUS_FORMAT_UNSPECIFIED;
        struct status_deferred_config {
	        enum wt_status_format status_format;
	        int show_branch;
	        enum ahead_behind_flags ahead_behind;
        } __status_deferred_config;
        struct {
            int __no_renames;                   // = -1;
	        const char *__rename_score_arg;     // = (const char *)-1;
	        struct wt_status __s;
        } cmd_status;
        struct {
            struct wt_status __s;
        } cmd_commit;
    } builtin_commit;
    struct {
        struct opts_commit_graph {
	        const char *obj_dir;
	        int reachable;
	        int stdin_packs;
	        int stdin_commits;
	        int append;
	        int split;
	        int shallow;
	        int progress;
	        int enable_changed_paths;
        } __opts;
        struct option __common_opts[2];
        int __read_replace_refs;
        struct commit_graph_opts __write_opts;
    } builtin_commit_graph;
    struct {
        const char *__sign_commit;
        struct {
            struct strbuf __buffer;             // = STRBUF_INIT;
        } cmd_commit_tree;
    } builtin_commit_tree;
    struct {
        char *__key;
        regex_t *__key_regexp;
        const char *__value_pattern;
        regex_t *__regexp;
        int __show_keys;
        int __omit_values;
        int __use_key_regexp;
        int __do_all;
        int __do_not_match;
        char __delim;                           // = '=';
        char __key_delim;                       // = ' ';
        char __term;                            // = '\n';
        int __use_global_config, __use_system_config, __use_local_config;
        int __use_worktree_config;
        struct git_config_source __given_config_source;
        int __actions, __type;
        char *__default_value;
        int __end_nul;
        int __respect_includes_opt;             // = -1;
        struct config_options __config_options;
        int __show_origin;
        int __show_scope;
        int __fixed_value;
        int __get_color_found;
        const char *__get_color_slot;
        const char *__get_colorbool_slot;
        char __parsed_color[COLOR_MAXLEN];
        int __get_colorbool_found;
        int __get_diff_color_found;
        int __get_color_ui_found;
        struct option __builtin_config_options[39];
    } builtin_config;
    struct {
        unsigned long __garbage;
        off_t __size_garbage;
        int __verbose;
        unsigned long __loose, __packed, __packed_loose;
        off_t __loose_size;
    } builtin_count_objects;
    struct {
        struct credential_cache_entry *__entries;
        int __entries_nr;
        int __entries_alloc;
        struct {
            timestamp_t __wait_for_entry_until;
        } check_expirations;
        struct {
            struct strbuf __item;               // = STRBUF_INIT;
        } read_request;
    } builtin_credential_cahce_daemon;
    struct {
        struct lock_file __credential_lock;
    } builtin_credential_store;
    struct {
        int __debug;
        int __all;
        int __tags;
        int __longformat;
        int __first_parent;
        int __abbrev;                           // = -1;
        int __max_candidates;                   // = 10;
        struct hashmap __names;
        int __have_util;
        struct string_list __patterns;          // = STRING_LIST_INIT_NODUP;
        struct string_list __exclude_patterns;  // = STRING_LIST_INIT_NODUP;
        int __always;
        const char *__suffix, *__dirty, *__broken;
        struct commit_names __commit_names;
        struct {
            int __label_width;                  // = -1;
        } describe_commit;
    } builtin_describe;
    struct {
        int __trust_exit_code;
        struct {
            char *__difftool_cmd, *__extcmd;
        } cmd_difftool;
    } builtin_difftool;
    struct {
        struct rev_info __log_tree_opt;
        struct {
            struct rev_info *__opt;             // = &log_tree_opt;
        } cmd_diff_tree;
    } builtin_diff_tree;
    struct {
        int __progress;
        enum { SIGNED_TAG_ABORT, VERBATIM, WARN, WARN_STRIP, STRIP } __signed_tag_mode; // = SIGNED_TAG_ABORT;
        enum { TAG_FILTERING_ABORT, DROP, REWRITE } __tag_of_filtered_mode;             // = TAG_FILTERING_ABORT;
        enum { REENCODE_ABORT, REENCODE_YES, REENCODE_NO } __reencode_mode;             // = REENCODE_ABORT;
        int __fake_missing_tagger;
        int __use_done_feature;
        int __no_data;
        int __full_tree;
        int __reference_excluded_commits;
        int __show_original_ids;
        int __mark_tags;
        struct string_list __extra_refs;        // = STRING_LIST_INIT_NODUP;
        struct string_list __tag_refs;          // = STRING_LIST_INIT_NODUP;
        struct refspec __refspecs;              // = REFSPEC_INIT_FETCH;
        int __anonymize;
        struct hashmap __anonymized_seeds;
        struct revision_sources __revision_sources;
        struct decoration __idnums;
        uint32_t __last_idnum;
        struct {
            int __counter;
        } show_progress;
        struct {
            int __counter;
        } anonymize_blob;
        struct {
            int __counter;
        } anonymize_path_component;
        struct {
            struct hashmap __paths;
		    struct strbuf __anon;               // = STRBUF_INIT;
        } print_path;
        struct {
            uint32_t __counter;                 // = -1;
        } generate_fake_oid;
        struct {
            struct hashmap __objs;
        } anonymize_oid;
        struct {
            int __counter;
        } anonymize_ref_component;
        struct {
            struct hashmap __refs;
	        struct strbuf __anon;               // = STRBUF_INIT;
        } anonymize_refname;
        struct {
            int __counter;
        } anonymize_commit_message;
        struct {
            int __counter;
        } anonymize_ident;
        struct {
            struct hashmap __idents;
	        struct strbuf __buffers[2];         // = { STRBUF_INIT, STRBUF_INIT };
	        unsigned __which_buffer;
        } anonymize_ident_line;
        struct {
            int __counter;
        } anonymize_tag;
    } builtin_fast_export;
    struct {
        unsigned long __max_depth;              // = 50;
        off_t __max_packsize;
        int __unpack_limit;                     // = 100;
        int __force_update;
        uintmax_t __alloc_count;
        uintmax_t __marks_set_count;
        uintmax_t __object_count_by_type[1 << TYPE_BITS];
        uintmax_t __duplicate_count_by_type[1 << TYPE_BITS];
        uintmax_t __delta_count_by_type[1 << TYPE_BITS];
        uintmax_t __delta_count_attempts_by_type[1 << TYPE_BITS];
        unsigned long __object_count;
        unsigned long __branch_count;
        unsigned long __branch_load_count;
        int __failure;
        FILE *__pack_edges;
        unsigned int __show_stats;              // = 1;
        int __global_argc;
        const char **__global_argv;
        struct mem_pool __fi_mem_pool;          // =  {NULL, 2*1024*1024 - sizeof(struct mp_block), 0};
        unsigned int __atom_table_sz;           // = 4451;
        unsigned int __atom_cnt;
        struct atom_str **__atom_table;
        struct pack_idx_option __pack_idx_opts;
        unsigned int __pack_id;
        struct hashfile *__pack_file;
        struct packed_git *__pack_data;
        struct packed_git **__all_packs;
        off_t __pack_size;
        unsigned int __object_entry_alloc;      // = 5000;
        struct object_entry_pool *__blocks;
        struct hashmap __object_table;
        struct mark_set *__marks;
        const char *__export_marks_file;
        const char *__import_marks_file;
        int __import_marks_file_from_stream;
        int __import_marks_file_ignore_missing;
        int __import_marks_file_done;
        int __relative_marks_paths;
        struct last_object __last_blob;         // = { STRBUF_INIT, 0, 0, 0 };
        unsigned int __tree_entry_alloc;        // = 1000;
        void *__avail_tree_entry;
        unsigned int __avail_tree_table_sz;     // = 100;
        struct avail_tree_content **__avail_tree_table;
        size_t __tree_entry_allocd;
        struct strbuf __old_tree;               // = STRBUF_INIT;
        struct strbuf __new_tree;               // = STRBUF_INIT;
        unsigned long __max_active_branches;    // = 5;
        unsigned long __cur_active_branches;
        unsigned long __branch_table_sz;        // = 1039;
        struct branch **__branch_table;
        struct branch *__active_branches;
        struct tag *__first_tag;
        struct tag *__last_tag;
        whenspec_type __whenspec;               // = WHENSPEC_RAW;
        struct strbuf __command_buf;            // = STRBUF_INIT;
        int __unread_command_buf;
        struct recent_command __cmd_hist;       // = {&cmd_hist, &cmd_hist, NULL};
        struct recent_command *__cmd_tail;      // = &cmd_hist;
        struct recent_command *__rc_free;
        unsigned int __cmd_save;                // = 100;
        uintmax_t __next_mark;
        struct strbuf __new_data;               // = STRBUF_INIT;
        int __seen_data_command;
        int __require_explicit_termination;
        int __allow_unsafe_features;
        volatile sig_atomic_t __checkpoint_requested;
        struct string_list __sub_marks_from;    // = STRING_LIST_INIT_DUP;
        struct string_list __sub_marks_to;      // = STRING_LIST_INIT_DUP;
        kh_oid_map_t *__sub_oid_map;
        int __cat_blob_fd;                      // = STDOUT_FILENO;
        struct {
            int __running;
        } end_packfile;
        struct {
            int __stdin_eof;
        } read_next_command;
        struct {
            struct strbuf __buf;                // = STRBUF_INIT;
        } parse_and_store_blob;
        struct {
            struct strbuf __uq;                 // = STRBUF_INIT;
        } file_change_m;
        struct {
            struct strbuf __uq;                 // = STRBUF_INIT;
        } file_change_d;
        struct {
            struct strbuf __s_uq;               // = STRBUF_INIT;
	        struct strbuf __d_uq;               // = STRBUF_INIT;
        } file_change_cr;
        struct {
            struct strbuf __uq;                 // = STRBUF_INIT;
        } note_change_n;
        struct {
            struct strbuf __msg;                // = STRBUF_INIT;
        } parse_new_commit;
        struct {
            struct strbuf __msg;                // = STRBUF_INIT;
        } parse_new_tag;
        struct {
            struct strbuf __line;               // = STRBUF_INIT;
        } print_ls;
        struct {
            struct strbuf __uq;                 // = STRBUF_INIT;
        } parse_ls;
    } builtin_fast_import;

    // src
};

void * __git_ctx(void);
#define git_ctx                 ((struct git_ctx_t *)__git_ctx())

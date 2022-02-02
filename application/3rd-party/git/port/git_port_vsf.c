#ifdef __VSF__

// implement mingw_spawnvpe will require __VSF_LINUX_CLASS_IMPLEMENT
#define __VSF_LINUX_CLASS_IMPLEMENT
#define __VSF_LINUX_FS_CLASS_IMPLEMENT
#include <unistd.h>
#include <fcntl.h>
#include <spawn.h>

#define __VSF_HEADER_SHOW_CURL_CTX__
//#include "3rd-party/curl/raw/include/curl/curl.h"
#include "3rd-party/curl/raw/src/tool_setup.h"

#include "./git_port_vsf.h"

#if 1
struct tm *localtime_r(const time_t *t, struct tm *result)
{
    return NULL;
}

struct tm *gmtime_r(const time_t *t, struct tm *result)
{
    return NULL;
}

int ftruncate(int fildes, off_t length)
{
    VSF_LINUX_ASSERT(false);
    return -1;
}

pid_t tcgetpgrp(int fd)
{
    return -1;
}

int tcsetpgrp(int fd, pid_t pgrp)
{
    return -1;
}

pid_t mingw_spawnvpe(const char *cmd, const char **argv, char **deltaenv,
            const char *dir, int fhin, int fhout, int fherr)
{
    pid_t pid;
    posix_spawn_file_actions_t actions;
    posix_spawnattr_t attr;

    vsf_linux_process_t *process = vsf_linux_get_cur_process();

    posix_spawnattr_init(&attr);
    posix_spawn_file_actions_init(&actions);
    if (fhin != STDIN_FILENO) {
        posix_spawn_file_actions_adddup2(&actions, fhin, STDIN_FILENO);
    }
    if (fhout != STDOUT_FILENO) {
        posix_spawn_file_actions_adddup2(&actions, fhout, STDOUT_FILENO);
    }
    if (fherr != STDERR_FILENO) {
        posix_spawn_file_actions_adddup2(&actions, fherr, STDERR_FILENO);
    }

    // close all none-stdio fds
    __vsf_dlist_foreach_unsafe(vsf_linux_fd_t, fd_node, &process->fd_list) {
        if (!(_->fd_flags & FD_CLOEXEC) && (_->fd > 2)) {
            posix_spawn_file_actions_addclose(&actions, _->fd);
        }
    }

    posix_spawn(&pid, cmd, &actions, &attr, (char * const *)argv, deltaenv);

    posix_spawn_file_actions_destroy(&actions);
    posix_spawnattr_destroy(&attr);
    return pid;
}
#endif

static int __git_lib_idx = -1;

struct git_lib_ctx_t {
    struct curl_lib_ctx_t curl_lib_ctx;
    struct git_ctx_t __git_ctx;
};

#define git_lib_ctx         ((struct git_lib_ctx_t *)vsf_linux_library_ctx(__git_lib_idx))

int vsf_linux_git_init(void)
{
    struct git_lib_ctx_t *ctx = calloc(1, sizeof(struct git_lib_ctx_t));
    if (NULL == ctx) { return -1; }
    int err = vsf_linux_library_init(&__git_lib_idx, ctx, free);
    if (err) { return err; }
    err = curl_lib_init(&ctx->curl_lib_ctx);
    if (err) { return err; }

#undef git_ctx
    struct git_ctx_t *git_ctx = __git_ctx();

    // initializer
    extern void print_and_abort (void);
    git_ctx->obstack.__obstack_alloc_failed_handler = print_and_abort;
#if defined(HAVE_DEV_TTY) || defined(GIT_WINDOWS_NATIVE)
#   ifdef HAVE_DEV_TTY
    git_ctx->terminal.__term_fd = -1;
#   endif
    git_ctx->terminal.git_terminal_prompt.__buf = (struct strbuf)STRBUF_INIT;
#endif
    git_ctx->refs_debug.__trace_refs = (struct trace_key)TRACE_KEY_INIT(REFS);
#if defined(NO_MMAP)
    git_ctx->refs_packed_backend.__mmap_strategy = MMAP_NONE;
#elif defined(MMAP_PREVENTS_DELETE)
    git_ctx->refs_packed_backend.__mmap_strategy = MMAP_TEMPORARY;
#else
    git_ctx->refs_packed_backend.__mmap_strategy = MMAP_OK;
#endif
    git_ctx->refs_packed_backend.packed_refs_lock.__timeout_value = 1000;
    extern const struct ref_storage_be refs_be_files;
    git_ctx->refs.__refs_backends = &refs_be_files;
    git_ctx->refs.get_files_ref_lock_timeout_ms.__timeout_ms = 100;
    git_ctx->refs.refs_ref_iterator_begin.__ref_paranoia = -1;
    git_ctx->refs.refs_resolve_ref_unsafe.__sb_refname = (struct strbuf)STRBUF_INIT;
    git_ctx->tr2_cmd_name.__tr2cmdname_hierarchy = (struct strbuf)STRBUF_INIT;
    git_ctx->tr2_dst.tr2_dst_want_warning.__tr2env_dst_debug = -1;
    git_ctx->tr2_sid.__tr2sid_buf = (struct strbuf)STRBUF_INIT;

static struct tr2_sysenv_entry __tr2_sysenv_settings[] = {
    [TR2_SYSENV_CFG_PARAM]     = { "GIT_TRACE2_CONFIG_PARAMS",
                       "trace2.configparams" },
    [TR2_SYSENV_ENV_VARS]      = { "GIT_TRACE2_ENV_VARS",
                       "trace2.envvars" },

    [TR2_SYSENV_DST_DEBUG]     = { "GIT_TRACE2_DST_DEBUG",
                       "trace2.destinationdebug" },

    [TR2_SYSENV_NORMAL]        = { "GIT_TRACE2",
                       "trace2.normaltarget" },
    [TR2_SYSENV_NORMAL_BRIEF]  = { "GIT_TRACE2_BRIEF",
                       "trace2.normalbrief" },

    [TR2_SYSENV_EVENT]         = { "GIT_TRACE2_EVENT",
                       "trace2.eventtarget" },
    [TR2_SYSENV_EVENT_BRIEF]   = { "GIT_TRACE2_EVENT_BRIEF",
                       "trace2.eventbrief" },
    [TR2_SYSENV_EVENT_NESTING] = { "GIT_TRACE2_EVENT_NESTING",
                       "trace2.eventnesting" },

    [TR2_SYSENV_PERF]          = { "GIT_TRACE2_PERF",
                       "trace2.perftarget" },
    [TR2_SYSENV_PERF_BRIEF]    = { "GIT_TRACE2_PERF_BRIEF",
                       "trace2.perfbrief" },

    [TR2_SYSENV_MAX_FILES]     = { "GIT_TRACE2_MAX_FILES",
                       "trace2.maxfiles" },
};
    if (dimof(__tr2_sysenv_settings) > dimof(git_ctx->tr2_sysenv.__tr2_sysenv_settings)) {
        VSF_LINUX_ASSERT(false);
        return -1;
    }
    memcpy(git_ctx->tr2_sysenv.__tr2_sysenv_settings, __tr2_sysenv_settings, sizeof(__tr2_sysenv_settings));

    git_ctx->__tr2_tgt_event.__tr2dst_event = (struct tr2_dst){ TR2_SYSENV_EVENT, 0, 0, 0, 0 };
    git_ctx->__tr2_tgt_event.__tr2env_event_max_nesting_levels = 2;
    git_ctx->__tr2_tgt_event.__tr2_tgt_event = __tr2_tgt_event;
    git_ctx->__tr2_tgt_event.__tr2_tgt_event.pdst = &git_ctx->__tr2_tgt_event.__tr2dst_event;
    git_ctx->__tr2_tgt_normal.__tr2dst_normal = (struct tr2_dst){ TR2_SYSENV_NORMAL, 0, 0, 0, 0 };
    git_ctx->__tr2_tgt_normal.__tr2_tgt_normal = __tr2_tgt_normal;
    git_ctx->__tr2_tgt_normal.__tr2_tgt_normal.pdst = &git_ctx->__tr2_tgt_normal.__tr2dst_normal;
    git_ctx->__tr2_tgt_perf.__tr2dst_perf = (struct tr2_dst){ TR2_SYSENV_PERF, 0, 0, 0, 0 };
    git_ctx->__tr2_tgt_perf.__tr2_tgt_perf = __tr2_tgt_perf;
    git_ctx->__tr2_tgt_perf.__tr2_tgt_perf.pdst = &git_ctx->__tr2_tgt_perf.__tr2dst_perf;
    git_ctx->trace2.__tr2_next_repo_id = 1;

struct tr2_tgt *__tr2_tgt_builtins[] =
{
    &tr2_tgt_normal,
    &tr2_tgt_perf,
    &tr2_tgt_event,
    NULL
};
    if (dimof(__tr2_tgt_builtins) > dimof(git_ctx->trace2.__tr2_tgt_builtins)) {
        VSF_LINUX_ASSERT(false);
        return -1;
    }
    memcpy(git_ctx->trace2.__tr2_tgt_builtins, __tr2_tgt_builtins, sizeof(__tr2_tgt_builtins));

    git_ctx->builtin_add.__warn_on_embedded_repo = 1;
    git_ctx->builtin_add.__addremove = ADDREMOVE_DEFAULT;
    git_ctx->builtin_add.__addremove_explicit = -1;
    git_ctx->builtin_am.msgnum.__sb = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_blame.format_time.__time_buf = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_blame.__abbrev = -1;
    git_ctx->builtin_blame.__ignore_revs_file_list = (struct string_list)STRING_LIST_INIT_NODUP;
    git_ctx->builtin_blame.__blame_date_mode = (struct date_mode){ DATE_ISO8601 };
    git_ctx->builtin_blame.__mailmap = (struct string_list)STRING_LIST_INIT_NODUP;
    git_ctx->builtin_branch.__output = (struct string_list)STRING_LIST_INIT_DUP;
    git_ctx->builtin_branch.__branch_use_color = -1;
    git_ctx->builtin_branch.quote_literal_for_format.__buf = (struct strbuf)STRBUF_INIT;

# define all_attrs (git_ctx->builtin_check_attr.__all_attrs)
# define cached_attrs (git_ctx->builtin_check_attr.__cached_attrs)
# define stdin_paths (git_ctx->builtin_check_attr.__stdin_paths)
# define nul_term_line (git_ctx->builtin_check_attr.__nul_term_line)
struct option __check_attr_options[] = {
    OPT_BOOL('a', "all", &all_attrs, N_("report all attributes set on file")),
    OPT_BOOL(0,  "cached", &cached_attrs, N_("use .gitattributes only from the index")),
    OPT_BOOL(0 , "stdin", &stdin_paths, N_("read file names from stdin")),
    OPT_BOOL('z', NULL, &nul_term_line, N_("terminate input and output records by a NUL character")),
    OPT_END()
};
    if (dimof(__check_attr_options) > dimof(git_ctx->builtin_check_attr.__check_attr_options)) {
        VSF_LINUX_ASSERT(false);
        return -1;
    }
    memcpy(git_ctx->builtin_check_attr.__check_attr_options, __check_attr_options, sizeof(__check_attr_options));

    git_ctx->builtin_checkout.__cb_option = 'b';
    git_ctx->builtin_checkout_index.__state = (struct checkout)CHECKOUT_INIT;
    git_ctx->builtin_clean.__force = -1;
    git_ctx->builtin_clean.__del_list = (struct string_list)STRING_LIST_INIT_DUP;
    git_ctx->builtin_clean.__clean_use_color = -1;
    git_ctx->builtin_clone.__option_single_branch = -1;
    git_ctx->builtin_clone.__option_local = -1;
    git_ctx->builtin_clone.__option_reject_shallow = -1;
    git_ctx->builtin_clone.__config_reject_shallow = -1;
    git_ctx->builtin_clone.__option_not = (struct string_list)STRING_LIST_INIT_NODUP;
    git_ctx->builtin_clone.__option_upload_pack = "git-upload-pack";
    git_ctx->builtin_clone.__option_progress = -1;
    git_ctx->builtin_clone.__option_config = (struct string_list)STRING_LIST_INIT_NODUP;
    git_ctx->builtin_clone.__option_required_reference = (struct string_list)STRING_LIST_INIT_NODUP;
    git_ctx->builtin_clone.__option_optional_reference = (struct string_list)STRING_LIST_INIT_NODUP;
    git_ctx->builtin_clone.__max_jobs = -1;
    git_ctx->builtin_clone.__option_recurse_submodules = (struct string_list)STRING_LIST_INIT_NODUP;
    git_ctx->builtin_clone.__server_options = (struct string_list)STRING_LIST_INIT_NODUP;
    git_ctx->builtin_commit.__edit_flag = -1;
    git_ctx->builtin_commit.__config_commit_verbose = -1;
    git_ctx->builtin_commit.__trailer_args = (struct strvec)STRVEC_INIT;
    git_ctx->builtin_commit.__use_editor = 1;
    git_ctx->builtin_commit.__include_status = 1;
    git_ctx->builtin_commit.__message = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_commit.__status_format = STATUS_FORMAT_UNSPECIFIED;
    git_ctx->builtin_commit.__status_deferred_config = (struct status_deferred_config) {
        STATUS_FORMAT_UNSPECIFIED,
        -1, /* unspecified */
        AHEAD_BEHIND_UNSPECIFIED,
    };
    git_ctx->builtin_commit.cmd_status.__no_renames = -1;
    git_ctx->builtin_commit.cmd_status.__rename_score_arg = (const char *)-1;

struct option __common_opts[] = {
	OPT_STRING(0, "object-dir", &git_ctx->builtin_commit_graph.__opts.obj_dir,
		   N_("dir"),
		   N_("the object directory to store the graph")),
	OPT_END()
};
    if (dimof(__common_opts) > dimof(git_ctx->builtin_commit_graph.__common_opts)) {
        VSF_LINUX_ASSERT(false);
        return -1;
    }
    memcpy(git_ctx->builtin_commit_graph.__common_opts, __common_opts, sizeof(__common_opts));

    git_ctx->builtin_commit_tree.cmd_commit_tree.__buffer = (struct strbuf)STRBUF_INIT;

# define key (git_ctx->builtin_config.__key)
# define key_regexp (git_ctx->builtin_config.__key_regexp)
# define value_pattern (git_ctx->builtin_config.__value_pattern)
# define regexp (git_ctx->builtin_config.__regexp)
# define show_keys (git_ctx->builtin_config.__show_keys)
# define omit_values (git_ctx->builtin_config.__omit_values)
# define use_key_regexp (git_ctx->builtin_config.__use_key_regexp)
# define do_all (git_ctx->builtin_config.__do_all)
# define do_not_match (git_ctx->builtin_config.__do_not_match)
# define delim (git_ctx->builtin_config.__delim)
# define key_delim (git_ctx->builtin_config.__key_delim)
# define term (git_ctx->builtin_config.__term)
# define use_global_config (git_ctx->builtin_config.__use_global_config)
# define use_system_config (git_ctx->builtin_config.__use_system_config)
# define use_local_config (git_ctx->builtin_config.__use_local_config)
# define use_worktree_config (git_ctx->builtin_config.__use_worktree_config)
# define given_config_source (git_ctx->builtin_config.__given_config_source)
# define actions (git_ctx->builtin_config.__actions)
# define type (git_ctx->builtin_config.__type)
# define default_value (git_ctx->builtin_config.__default_value)
# define end_nul (git_ctx->builtin_config.__end_nul)
# define respect_includes_opt (git_ctx->builtin_config.__respect_includes_opt)
# define config_options (git_ctx->builtin_config.__config_options)
# define show_origin (git_ctx->builtin_config.__show_origin)
# define show_scope (git_ctx->builtin_config.__show_scope)
# define fixed_value (git_ctx->builtin_config.__fixed_value)
#define ACTION_GET (1<<0)
#define ACTION_GET_ALL (1<<1)
#define ACTION_GET_REGEXP (1<<2)
#define ACTION_REPLACE_ALL (1<<3)
#define ACTION_ADD (1<<4)
#define ACTION_UNSET (1<<5)
#define ACTION_UNSET_ALL (1<<6)
#define ACTION_RENAME_SECTION (1<<7)
#define ACTION_REMOVE_SECTION (1<<8)
#define ACTION_LIST (1<<9)
#define ACTION_EDIT (1<<10)
#define ACTION_SET (1<<11)
#define ACTION_SET_ALL (1<<12)
#define ACTION_GET_COLOR (1<<13)
#define ACTION_GET_COLORBOOL (1<<14)
#define ACTION_GET_URLMATCH (1<<15)
#define TYPE_BOOL		1
#define TYPE_INT		2
#define TYPE_BOOL_OR_INT	3
#define TYPE_PATH		4
#define TYPE_EXPIRY_DATE	5
#define TYPE_COLOR		6
#define TYPE_BOOL_OR_STR	7
#define OPT_CALLBACK_VALUE(s, l, v, h, i) \
	{ OPTION_CALLBACK, (s), (l), (v), NULL, (h), PARSE_OPT_NOARG | \
	PARSE_OPT_NONEG, option_parse_type, (i) }
extern int option_parse_type(const struct option *opt, const char *arg, int unset);
struct option __builtin_config_options[] = {
	OPT_GROUP(N_("Config file location")),
	OPT_BOOL(0, "global", &use_global_config, N_("use global config file")),
	OPT_BOOL(0, "system", &use_system_config, N_("use system config file")),
	OPT_BOOL(0, "local", &use_local_config, N_("use repository config file")),
	OPT_BOOL(0, "worktree", &use_worktree_config, N_("use per-worktree config file")),
	OPT_STRING('f', "file", &given_config_source.file, N_("file"), N_("use given config file")),
	OPT_STRING(0, "blob", &given_config_source.blob, N_("blob-id"), N_("read config from given blob object")),
	OPT_GROUP(N_("Action")),
	OPT_BIT(0, "get", &actions, N_("get value: name [value-pattern]"), ACTION_GET),
	OPT_BIT(0, "get-all", &actions, N_("get all values: key [value-pattern]"), ACTION_GET_ALL),
	OPT_BIT(0, "get-regexp", &actions, N_("get values for regexp: name-regex [value-pattern]"), ACTION_GET_REGEXP),
	OPT_BIT(0, "get-urlmatch", &actions, N_("get value specific for the URL: section[.var] URL"), ACTION_GET_URLMATCH),
	OPT_BIT(0, "replace-all", &actions, N_("replace all matching variables: name value [value-pattern]"), ACTION_REPLACE_ALL),
	OPT_BIT(0, "add", &actions, N_("add a new variable: name value"), ACTION_ADD),
	OPT_BIT(0, "unset", &actions, N_("remove a variable: name [value-pattern]"), ACTION_UNSET),
	OPT_BIT(0, "unset-all", &actions, N_("remove all matches: name [value-pattern]"), ACTION_UNSET_ALL),
	OPT_BIT(0, "rename-section", &actions, N_("rename section: old-name new-name"), ACTION_RENAME_SECTION),
	OPT_BIT(0, "remove-section", &actions, N_("remove a section: name"), ACTION_REMOVE_SECTION),
	OPT_BIT('l', "list", &actions, N_("list all"), ACTION_LIST),
	OPT_BOOL(0, "fixed-value", &fixed_value, N_("use string equality when comparing values to 'value-pattern'")),
	OPT_BIT('e', "edit", &actions, N_("open an editor"), ACTION_EDIT),
	OPT_BIT(0, "get-color", &actions, N_("find the color configured: slot [default]"), ACTION_GET_COLOR),
	OPT_BIT(0, "get-colorbool", &actions, N_("find the color setting: slot [stdout-is-tty]"), ACTION_GET_COLORBOOL),
	OPT_GROUP(N_("Type")),
	OPT_CALLBACK('t', "type", &type, "", N_("value is given this type"), option_parse_type),
	OPT_CALLBACK_VALUE(0, "bool", &type, N_("value is \"true\" or \"false\""), TYPE_BOOL),
	OPT_CALLBACK_VALUE(0, "int", &type, N_("value is decimal number"), TYPE_INT),
	OPT_CALLBACK_VALUE(0, "bool-or-int", &type, N_("value is --bool or --int"), TYPE_BOOL_OR_INT),
	OPT_CALLBACK_VALUE(0, "bool-or-str", &type, N_("value is --bool or string"), TYPE_BOOL_OR_STR),
	OPT_CALLBACK_VALUE(0, "path", &type, N_("value is a path (file or directory name)"), TYPE_PATH),
	OPT_CALLBACK_VALUE(0, "expiry-date", &type, N_("value is an expiry date"), TYPE_EXPIRY_DATE),
	OPT_GROUP(N_("Other")),
	OPT_BOOL('z', "null", &end_nul, N_("terminate values with NUL byte")),
	OPT_BOOL(0, "name-only", &omit_values, N_("show variable names only")),
	OPT_BOOL(0, "includes", &respect_includes_opt, N_("respect include directives on lookup")),
	OPT_BOOL(0, "show-origin", &show_origin, N_("show origin of config (file, standard input, blob, command line)")),
	OPT_BOOL(0, "show-scope", &show_scope, N_("show scope of config (worktree, local, global, system, command)")),
	OPT_STRING(0, "default", &default_value, N_("value"), N_("with --get, use default value when missing entry")),
	OPT_END(),
};
    if (dimof(__builtin_config_options) > dimof(git_ctx->builtin_config.__builtin_config_options)) {
        VSF_LINUX_ASSERT(false);
        return -1;
    }
    memcpy(git_ctx->builtin_config.__builtin_config_options, __builtin_config_options, sizeof(__builtin_config_options));

    git_ctx->builtin_config.__delim = '=';
    git_ctx->builtin_config.__key_delim = ' ';
    git_ctx->builtin_config.__term = '\n';
    git_ctx->builtin_config.__respect_includes_opt = -1;
    git_ctx->builtin_credential_cahce_daemon.read_request.__item = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_describe.__abbrev = -1;
    git_ctx->builtin_describe.__max_candidates = 10;
    git_ctx->builtin_describe.__patterns = (struct string_list)STRING_LIST_INIT_NODUP;
    git_ctx->builtin_describe.__exclude_patterns = (struct string_list)STRING_LIST_INIT_NODUP;
    git_ctx->builtin_describe.describe_commit.__label_width = -1;
    git_ctx->builtin_diff_tree.cmd_diff_tree.__opt = &git_ctx->builtin_diff_tree.__log_tree_opt;
    git_ctx->builtin_fast_export.print_path.__anon = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_export.generate_fake_oid.__counter = -1;
    git_ctx->builtin_fast_export.anonymize_refname.__anon = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_export.anonymize_ident_line.__buffers[0] = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_export.anonymize_ident_line.__buffers[1] = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_export.__signed_tag_mode = SIGNED_TAG_ABORT;
    git_ctx->builtin_fast_export.__tag_of_filtered_mode = TAG_FILTERING_ABORT;
    git_ctx->builtin_fast_export.__reencode_mode = REENCODE_ABORT;
    git_ctx->builtin_fast_export.__extra_refs = (struct string_list)STRING_LIST_INIT_NODUP;
    git_ctx->builtin_fast_export.__tag_refs = (struct string_list)STRING_LIST_INIT_NODUP;
    git_ctx->builtin_fast_export.__refspecs = (struct refspec)REFSPEC_INIT_FETCH;
    git_ctx->builtin_fast_import.__max_depth = 50;
    git_ctx->builtin_fast_import.__unpack_limit = 100;
    git_ctx->builtin_fast_import.__show_stats = 1;
    git_ctx->builtin_fast_import.__fi_mem_pool = (struct mem_pool){NULL, 2*1024*1024 - sizeof(struct mp_block), 0};
    git_ctx->builtin_fast_import.__atom_table_sz = 4451;
    git_ctx->builtin_fast_import.__object_entry_alloc = 5000;
    git_ctx->builtin_fast_import.__last_blob = (struct last_object){ STRBUF_INIT, 0, 0, 0 };
    git_ctx->builtin_fast_import.__tree_entry_alloc = 1000;
    git_ctx->builtin_fast_import.__avail_tree_table_sz = 100;
    git_ctx->builtin_fast_import.__old_tree = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_import.__new_tree = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_import.__max_active_branches = 5;
    git_ctx->builtin_fast_import.__branch_table_sz = 1039;
    git_ctx->builtin_fast_import.__whenspec = WHENSPEC_RAW;
    git_ctx->builtin_fast_import.__command_buf = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_import.__cmd_hist = (struct recent_command){&git_ctx->builtin_fast_import.__cmd_hist, &git_ctx->builtin_fast_import.__cmd_hist, NULL};
    git_ctx->builtin_fast_import.__cmd_tail = &git_ctx->builtin_fast_import.__cmd_hist;
    git_ctx->builtin_fast_import.__cmd_save = 100;
    git_ctx->builtin_fast_import.__new_data = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_import.__sub_marks_from = (struct string_list)STRING_LIST_INIT_DUP;
    git_ctx->builtin_fast_import.__sub_marks_to = (struct string_list)STRING_LIST_INIT_DUP;
    git_ctx->builtin_fast_import.__cat_blob_fd = STDOUT_FILENO;
    git_ctx->builtin_fast_import.parse_and_store_blob.__buf = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_import.file_change_m.__uq = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_import.file_change_d.__uq = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_import.file_change_cr.__s_uq = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_import.file_change_cr.__d_uq = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_import.note_change_n.__uq = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_import.parse_new_commit.__msg = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_import.parse_new_tag.__msg = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_import.print_ls.__line = (struct strbuf)STRBUF_INIT;
    git_ctx->builtin_fast_import.parse_ls.__uq = (struct strbuf)STRBUF_INIT;

    return err;
}

void * __git_ctx(void)
{
    return &((struct git_lib_ctx_t *)vsf_linux_library_ctx(__git_lib_idx))->__git_ctx;
}

#endif      // __VSF__

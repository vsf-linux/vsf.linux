#ifndef __GIT_CONFIG__
#define __GIT_CONFIG__

#include "hal/arch/vsf_arch.h"
#define bswap32         bswap_32
#define bswap64         bswap_64

#undef _WIN32
#undef _MSC_VER

// configurations not supported:
// regex:
//  RE_ENABLE_I18N
//  _REGEX_RE_COMP      BSD 4.2 compatible
//  _LIBC
// git:
//  HAVE_DEV_TTY

#define NO_MBSUPPORT
#define GAWK
#define NO_ICONV
#define NO_OPENSSL
#define NO_MMAP
#define NO_GETTEXT
#define NO_STRCASESTR
#define NO_MEMMEM
#define NO_POSIX_GOODIES
#define NO_SYMLINK_HEAD
#define NO_REGEX
#define GIT_HTML_PATH "share/doc/git-doc"
#define GIT_MAN_PATH "share/man"
#define GIT_INFO_PATH "share/info"
#define PAGER_ENV "LESS=FRX LV=-c"
#define FALLBACK_RUNTIME_PREFIX "/etc/"
#define GIT_EXEC_PATH "/sbin"
#define ETC_GITATTRIBUTES "/etc/gitattributes"
#define ETC_GITCONFIG "/etc/gitconfig"
#define DIR_HAS_BSD_GROUP_SEMANTICS
#define GIT_HOST_CPU "vsf32f103"
#define GIT_VERSION "1.0.0"
#define GIT_BUILT_FROM_COMMIT "12314124"
#define GIT_USER_AGENT ""

#undef main
#define main            git_main
#define cmd_rset        git_cmd_reset



#define __vsf_git_mod_name(__mod_name)      vsf ## _ ## __mod_name
#define vsf_git_mod_name(__mod_name)        __vsf_git_mod_name(__mod_name)
#define declare_vsf_git_mod(__mod_name)                                         \
        extern const vsf_linux_dynlib_mod_t vsf_git_mod_name(__mod_name);
#define vsf_git_ctx(__mod_name)                                                 \
        (vsf_linux_dynlib_ctx(&vsf_git_mod_name(__mod_name)))
#define define_vsf_git_mod(__mod_name, __mod_size, __mod_idx, __mod_init)       \
        const vsf_linux_dynlib_mod_t vsf_git_mod_name(__mod_name) = {           \
            .lib_idx            = &vsf_git_dynlib_idx,                          \
            .mod_idx            = (__mod_idx),                                  \
            .module_num         = VSF_GIT_MOD_NUM,                              \
            .bss_size           = VSF_GIT_BSS_SIZE,                             \
            .mod_size           = (__mod_size),                                 \
            .init               = (__mod_init),                                 \
        };
#define VSF_GIT_BSS_SIZE       32768
enum {
    GIT_MOD_OBSTACK = 0,

    GIT_MOD_EWAH_BITMAP,

    GIT_MOD_NEGOTIATOR_DEFAULT,
    GIT_MOD_NEGOTIATOR_SKIPPING,

    GIT_MOD_REFS_DEBUG,
    GIT_MOD_REFS_ITERATOR,
    GIT_MOD_REFS_PACKED_BACKEND,

    GIT_MOD_TRACE2_CFG,
    GIT_MOD_TRACE2_CMD_NAME,
    GIT_MOD_TRACE2_DST,
    GIT_MOD_TRACE2_SID,
    GIT_MOD_TRACE2_SYSENV,
    GIT_MOD_TRACE2_TGT,
    GIT_MOD_TRACE2_TGT_EVENT,
    GIT_MOD_TRACE2_TGT_NORMAL,
    GIT_MOD_TRACE2_TGT_PERF,
    GIT_MOD_TRACE2_TLS,

    GIT_MOD_BUILTIN_ADD,
    GIT_MOD_BUILTIN_AM,
    GIT_MOD_BUILTIN_BISECT_HELPER,
    GIT_MOD_BUILTIN_BLAME,

    GIT_MOD_ATTR,
    GIT_MOD_BISECT,
    GIT_MOD_BRANCH,
    GIT_MOD_REBASE,
    GIT_MOD_DIR,
    GIT_MOD_RERERE_PUBLIC,
    GIT_MOD_RERERE,
    GIT_MOD_SEQUENCER,

    VSF_GIT_MOD_NUM,
};
extern int vsf_git_dynlib_idx;

#endif      // __GIT_CONFIG__

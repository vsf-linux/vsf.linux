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

#endif      // __GIT_CONFIG__

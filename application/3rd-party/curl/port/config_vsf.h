#ifndef __CURL_CONFIG_VSF_H__
#define __CURL_CONFIG_VSF_H__

#undef _MSC_VER
#undef _WIN32
#undef _UNICODE
#undef main
#define main                    curl_main

#ifdef __WIN__
// clang in visual stdio does not have __STDC__, but it's necessary in curl
#   define __STDC__             1
#endif

#define OS                      "vsf"
#define BSD
#define __OpenBSD__
#define HAVE_VARIADIC_MACROS_C99
#define USE_SSL
#define USE_MBEDTLS
#define USE_THREADS_POSIX
#define CURL_STATICLIB
#define CURL_STRICTER
#define HAVE_ERRNO_H
#define HAVE_SYS_TYPES_H
#define HAVE_SYS_STAT_H
#define HAVE_SYS_TIME_H
#define HAVE_SYS_SOCKET_H
#define HAVE_PTHREAD_H
#define HAVE_TERMIOS_H
#define HAVE_UNISTD_H
#define HAVE_LIBGEN_H
#define HAVE_ARPA_INET_H
#define HAVE_ASSERT_H
#define HAVE_FCNTL_H
#define HAVE_NETDB_H
#define HAVE_POLL_H
#define HAVE_NETINET_IN_H
#define HAVE_NETINET_TCP_H
#define HAVE_NET_IF_H
#define HAVE_LONGLONG
#define HAVE_BOOL_T
#define HAVE_STRUCT_TIMEVAL
#define HAVE_STRUCT_POLLFD
//#define HAVE_SELECT
#define HAVE_POLL_FINE
#define HAVE_GETTIMEOFDAY
#define HAVE_SOCKET
#define HAVE_STRDUP
#define HAVE_STRTOLL
#define HAVE_STRICMP
#define HAVE_BASENAME
#define HAVE_STRUCT_IN6_ADDR
#define HAVE_RECV
#   define RECV_TYPE_RETV       int
#   define RECV_TYPE_ARG1       int
#   define RECV_TYPE_ARG2       void *
#   define RECV_TYPE_ARG3       size_t
#   define RECV_TYPE_ARG4       int
#define HAVE_RECVFROM
#   define RECVFROM_TYPE_RETV   ssize_t
#   define RECVFROM_TYPE_ARG1   int
#   define RECVFROM_TYPE_ARG2   void
#   define RECVFROM_TYPE_ARG3   size_t
#   define RECVFROM_TYPE_ARG4   int
#   define RECVFROM_TYPE_ARG5   struct sockaddr
#   define RECVFROM_TYPE_ARG6   socklen_t *
#   define RECVFROM_TYPE_ARG2_IS_VOID 1
#define HAVE_SEND
#   define SEND_TYPE_RETV       int
#   define SEND_TYPE_ARG1       int
#   define SEND_QUAL_ARG2       const
#   define SEND_TYPE_ARG2       void *
#   define SEND_TYPE_ARG3       size_t
#   define SEND_TYPE_ARG4       int
#define HAVE_SETSOCKOPT_SO_NONBLOCK

#define SIZEOF_INT              4
#define SIZEOF_LONG             4
#define SIZEOF_LONG_DOUBLE      8
#define SIZEOF_SHORT            2
#define SIZEOF_SIZE_T           4
#define SIZEOF_TIME_T           8
#define SIZEOF_OFF_T            4
#define SIZEOF_CURL_OFF_T       4

// CURL disable
#include "3rd-party/curl/curl_cfg_disable.h"

#endif      // __CURL_CONFIG_VSF_H__

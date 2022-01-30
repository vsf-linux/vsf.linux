
#ifdef CURLDEBUG
#   error CURLDEBUG is not supported
#endif

#ifdef USE_MBEDTLS
#   ifdef USE_THREADS_POSIX
#       include <pthread.h>
#       include <mbedtls/entropy.h>
#   endif
#endif

#ifdef HAVE_TERMIOS_H
#   include <termios.h>
#elif defined(HAVE_TERMIO_H)
#   include <termio.h>
#endif

struct speedcount {
  curl_off_t dl;
  curl_off_t ul;
  struct timeval stamp;
};
#define SPEEDCNT 10

struct curl_ctx_t {
    struct {
        unsigned int __initialized;
        long __init_flags;
#ifdef DEBUGBUILD
        char *__leakpointer;
#endif

        curl_malloc_callback __Curl_cmalloc;
        curl_free_callback __Curl_cfree;
        curl_realloc_callback __Curl_crealloc;
        curl_strdup_callback __Curl_cstrdup;
        curl_calloc_callback __Curl_ccalloc;
    } easy;

    struct {
        FILE *__keylog_file_fp;
    } keylog;

#ifdef USE_MBEDTLS
    struct {
#   ifdef USE_THREADS_POSIX
        mbedtls_entropy_context __ts_entropy;
        int __entropy_init_initialized;
        pthread_mutex_t *__mutex_buf;
#   endif
    } mbedtls;
#endif

    struct {
        bool __init_ssl;

        struct {
            const struct Curl_ssl *__selected;
            char __backends[200];
            size_t __backends_len;
        } multissl_version;
    } vtls;

    struct {
        struct {
            unsigned int __randseed;
            bool __seeded;
        } randit;
    } rand;

    struct {
        curl_version_info_data __version_info;

        struct {
#undef __out
            char __out[300];
        } curl_version;
        struct {
#ifdef USE_SSH
            char __ssh_buffer[80];
#endif
#ifdef USE_SSL
#ifdef CURL_WITH_MULTI_SSL
            char __ssl_buffer[200];
#else
            char __ssl_buffer[80];
#endif
#endif
#ifdef HAVE_BROTLI
            char __brotli_buffer[80];
#endif
#ifdef HAVE_ZSTD
            char __zstd_buffer[80];
#endif
        } curl_version_info;
    } version;

    struct {
        struct {
            time_t __epoch_offset;
            int __known_offset;
            bool __newl;
            bool __traced_data;
        } tool_debug_cb;
    } tool_cb_dbg;

#ifdef CURL_DOES_CONVERSIONS
    struct {
        iconv_t __inbound_cd;       //  = (iconv_t)-1;
        iconv_t __outbound_cd;      // = (iconv_t)-1;
    } tool_convert;
#endif

    struct {
        struct {
#ifdef HAVE_TERMIOS_H
            struct termios __withecho;
            struct termios __noecho;
#elif defined(HAVE_TERMIO_H)
            struct termio __withecho;
            struct termio __noecho;
#endif
        } ttyecho;
    } tool_getpass;

    struct {
        struct per_transfer *__transfers;
        struct per_transfer *__transfersl;
        long __all_added;

        struct {
            bool __warn_more_options;
        } single_transfer;
    } tool_operate;

    struct {
        struct {
            int __outnum;
        } new_getout;
    } tool_paramhlp;

    struct {
        curl_off_t __all_dltotal;
        curl_off_t __all_ultotal;
        curl_off_t __all_dlalready;
        curl_off_t __all_ulalready;
        curl_off_t __all_xfers;

        unsigned int __speedindex;
        bool __indexwrapped;
        struct speedcount __speedstore[SPEEDCNT];

        struct {
            struct timeval __stamp;
            bool __header;
        } progress_meter;
    } tool_progress;
};

void * __curl_ctx(void);
#define curl_ctx                ((struct curl_ctx_t *)__curl_ctx())

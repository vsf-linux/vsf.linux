#include <openssl/crypto.h>

struct openssl_ctx_t {
    struct {
        int __stopped;
        CRYPTO_ONCE __ssl_base;                 // = CRYPTO_ONCE_STATIC_INIT;
        int __ssl_base_inited;
        CRYPTO_ONCE __ssl_strings;              // = CRYPTO_ONCE_STATIC_INIT;
        int __ssl_strings_inited;

        struct {
            int __stoperrset;
        } OPENSSL_init_ssl;
    } ssl_init;

    struct {
        int __stopped;
        uint64_t __optsdone;
        void *__stop_handlers;
        CRYPTO_RWLOCK *__init_lock;
        CRYPTO_THREAD_LOCAL __in_init_config_local;

        CRYPTO_ONCE __base;                     //  = CRYPTO_ONCE_STATIC_INIT;
        int __base_inited;
        CRYPTO_ONCE __register_atexit;          // = CRYPTO_ONCE_STATIC_INIT;
        CRYPTO_ONCE __load_crypto_nodelete;     // = CRYPTO_ONCE_STATIC_INIT;
        CRYPTO_ONCE __load_crypto_strings;      // = CRYPTO_ONCE_STATIC_INIT;
        int __load_crypto_strings_inited;
        CRYPTO_ONCE __add_all_ciphers;          // = CRYPTO_ONCE_STATIC_INIT;
        CRYPTO_ONCE __add_all_digests;          // = CRYPTO_ONCE_STATIC_INIT;
        CRYPTO_ONCE __config;                   // = CRYPTO_ONCE_STATIC_INIT;
        int __config_inited;
        const OPENSSL_INIT_SETTINGS *__conf_settings;
        CRYPTO_ONCE __async;                    // = CRYPTO_ONCE_STATIC_INIT;
        int __async_inited;
#ifndef OPENSSL_NO_ENGINE
        CRYPTO_ONCE __engine_openssl;           // = CRYPTO_ONCE_STATIC_INIT;
# ifndef OPENSSL_NO_RDRAND
        CRYPTO_ONCE __engine_rdrand;            // = CRYPTO_ONCE_STATIC_INIT;
# endif
        CRYPTO_ONCE __engine_dynamic;           // = CRYPTO_ONCE_STATIC_INIT;
# ifndef OPENSSL_NO_STATIC_ENGINE
#  ifndef OPENSSL_NO_DEVCRYPTOENG
        CRYPTO_ONCE __engine_devcrypto;         // = CRYPTO_ONCE_STATIC_INIT;
#  endif
#  if !defined(OPENSSL_NO_PADLOCKENG)
        CRYPTO_ONCE __engine_padlock;           // = CRYPTO_ONCE_STATIC_INIT;
#  endif
#  if defined(OPENSSL_SYS_WIN32) && !defined(OPENSSL_NO_CAPIENG)
        CRYPTO_ONCE __engine_capi;              // = CRYPTO_ONCE_STATIC_INIT;
#  endif
#  if !defined(OPENSSL_NO_AFALGENG)
        CRYPTO_ONCE __engine_afalg;             // = CRYPTO_ONCE_STATIC_INIT;
#  endif
# endif
#endif
    } crypto_init;

#if !defined(OPENSSL_THREADS) || defined(CRYPTO_TDEBUG)
    struct {
        void *__thread_local_storage[256];
        struct {
            unsigned int __thread_local_key;
        } CRYPTO_THREAD_init_local;
    } threads_none;
#endif
};

#ifdef __VSF_HEADER_SHOW_OPENSSL_CTX__
#undef __VSF_HEADER_SHOW_OPENSSL_CTX__

struct openssl_lib_ctx_t {
    struct openssl_ctx_t __openssl_ctx;
};

extern int openssl_lib_init(struct openssl_lib_ctx_t *ctx);
#endif      // __VSF_HEADER_SHOW_OPENSSL_CTX__

void * __openssl_ctx(void);
#define openssl_ctx             ((struct openssl_ctx_t *)__openssl_ctx())

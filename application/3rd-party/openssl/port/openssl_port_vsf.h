#include <openssl/crypto.h>

struct openssl_ctx_t {
    struct {
        int stopped;
        CRYPTO_ONCE ssl_base;       // = CRYPTO_ONCE_STATIC_INIT;
        int ssl_base_inited;
        CRYPTO_ONCE ssl_strings;    // = CRYPTO_ONCE_STATIC_INIT;
        int ssl_strings_inited;

        struct {
            int stoperrset;
        } OPENSSL_init_ssl;
    } ssl_init;
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

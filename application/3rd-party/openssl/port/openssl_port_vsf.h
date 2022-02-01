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

void * __openssl_ctx(void);
#define openssl_ctx             ((struct openssl_ctx_t *)__openssl_ctx())

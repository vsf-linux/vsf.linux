#ifdef __VSF__

#include "vsf.h"
#include "./openssl_port_vsf.h"

static int __openssl_lib_idx = -1;

struct openssl_lib_ctx_t {
    struct openssl_ctx_t __openssl_ctx;
};

#define openssl_lib_ctx     ((struct openssl_lib_ctx_t *)vsf_linux_library_ctx(__openssl_lib_idx))

int vsf_linux_openssl_init(void)
{
    struct openssl_lib_ctx_t *ctx = calloc(1, sizeof(struct openssl_lib_ctx_t));
    if (NULL == ctx) { return -1; }
    int err = vsf_linux_library_init(&__openssl_lib_idx, ctx);
    if (err) { return err; }

    // initializer
    openssl_ctx->ssl_init.ssl_base = CRYPTO_ONCE_STATIC_INIT;
    openssl_ctx->ssl_init.ssl_strings = CRYPTO_ONCE_STATIC_INIT;

    return err;
}

void * __openssl_ctx(void)
{
    return &((struct openssl_lib_ctx_t *)vsf_linux_library_ctx(__openssl_lib_idx))->__openssl_ctx;
}

#endif      // __VSF__

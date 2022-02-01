#ifdef __VSF__

#include "vsf.h"

#define __VSF_HEADER_SHOW_OPENSSL_CTX__
#include "./openssl_port_vsf.h"

static int __openssl_lib_idx = -1;

#define openssl_lib_ctx     ((struct openssl_lib_ctx_t *)vsf_linux_library_ctx(__openssl_lib_idx))

int openssl_lib_init(struct openssl_lib_ctx_t *ctx)
{
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

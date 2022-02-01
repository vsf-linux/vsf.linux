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
    openssl_ctx->ssl_init.__ssl_base = CRYPTO_ONCE_STATIC_INIT;
    openssl_ctx->ssl_init.__ssl_strings = CRYPTO_ONCE_STATIC_INIT;
    openssl_ctx->crypto_init.__base = CRYPTO_ONCE_STATIC_INIT;
    openssl_ctx->crypto_init.__register_atexit = CRYPTO_ONCE_STATIC_INIT;
    openssl_ctx->crypto_init.__load_crypto_nodelete = CRYPTO_ONCE_STATIC_INIT;
    openssl_ctx->crypto_init.__load_crypto_strings = CRYPTO_ONCE_STATIC_INIT;
    openssl_ctx->crypto_init.__add_all_ciphers = CRYPTO_ONCE_STATIC_INIT;
    openssl_ctx->crypto_init.__add_all_digests = CRYPTO_ONCE_STATIC_INIT;
    openssl_ctx->crypto_init.__config = CRYPTO_ONCE_STATIC_INIT;
    openssl_ctx->crypto_init.__async = CRYPTO_ONCE_STATIC_INIT;
#ifndef OPENSSL_NO_ENGINE
    openssl_ctx->crypto_init.__engine_openssl = CRYPTO_ONCE_STATIC_INIT;
# ifndef OPENSSL_NO_RDRAND
    openssl_ctx->crypto_init.__engine_rdrand = CRYPTO_ONCE_STATIC_INIT;
# endif
    openssl_ctx->crypto_init.__engine_dynamic = CRYPTO_ONCE_STATIC_INIT;
# ifndef OPENSSL_NO_STATIC_ENGINE
#  ifndef OPENSSL_NO_DEVCRYPTOENG
    openssl_ctx->crypto_init.__engine_devcrypto = CRYPTO_ONCE_STATIC_INIT;
#  endif
#  if !defined(OPENSSL_NO_PADLOCKENG)
    openssl_ctx->crypto_init.__engine_padlock = CRYPTO_ONCE_STATIC_INIT;
#  endif
#  if defined(OPENSSL_SYS_WIN32) && !defined(OPENSSL_NO_CAPIENG)
    openssl_ctx->crypto_init.__engine_capi = CRYPTO_ONCE_STATIC_INIT;
#  endif
#  if !defined(OPENSSL_NO_AFALGENG)
    openssl_ctx->crypto_init.__engine_afalg = CRYPTO_ONCE_STATIC_INIT;
#  endif
# endif
#endif

    return err;
}

void * __openssl_ctx(void)
{
    return &((struct openssl_lib_ctx_t *)vsf_linux_library_ctx(__openssl_lib_idx))->__openssl_ctx;
}

#endif      // __VSF__

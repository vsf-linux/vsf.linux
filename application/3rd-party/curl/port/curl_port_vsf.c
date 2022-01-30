#ifdef __VSF__

#include "vsf.h"
#include "3rd-party/curl/raw/src/tool_setup.h"

static int __curl_lib_idx = -1;

struct curl_lib_ctx_t {
    struct curl_ctx_t __curl_ctx;
};

#define curl_lib_ctx     ((struct curl_lib_ctx_t *)vsf_linux_library_ctx(__curl_lib_idx))

void hugehelp(void)
{
  puts("This is a silly replacement for the actual file.");
}


int vsf_linux_curl_init(void)
{
    struct curl_lib_ctx_t *ctx = calloc(1, sizeof(struct curl_lib_ctx_t));
    if (NULL == ctx) { return -1; }
    int err = vsf_linux_library_init(&__curl_lib_idx, ctx);
    if (err) { return err; }

    // initializer
    curl_ctx->easy.__Curl_cmalloc = (curl_malloc_callback)vsf_heap_malloc;
    curl_ctx->easy.__Curl_cfree = (curl_free_callback)vsf_heap_free;
    curl_ctx->easy.__Curl_crealloc = (curl_realloc_callback)vsf_heap_realloc;
    curl_ctx->easy.__Curl_cstrdup = (curl_strdup_callback)vsf_heap_strdup;
    curl_ctx->easy.__Curl_ccalloc = (curl_calloc_callback)vsf_heap_calloc;

#ifdef CURL_DOES_CONVERSIONS
    curl_ctx->tool_convert.__inbound_cd = (iconv_t)-1;
    curl_ctx->tool_convert.__outbound_cd = (iconv_t)-1;
#endif

extern const char * const protocols[];
    curl_ctx->version.__version_info = (curl_version_info_data) {
  CURLVERSION_NOW,
  LIBCURL_VERSION,
  LIBCURL_VERSION_NUM,
  OS, /* as found by configure or set by hand at build-time */
  0 /* features is 0 by default */
#ifdef ENABLE_IPV6
  | CURL_VERSION_IPV6
#endif
#ifdef USE_SSL
  | CURL_VERSION_SSL
#endif
#ifdef USE_NTLM
  | CURL_VERSION_NTLM
#endif
#if !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM) && \
  defined(NTLM_WB_ENABLED)
  | CURL_VERSION_NTLM_WB
#endif
#ifdef USE_SPNEGO
  | CURL_VERSION_SPNEGO
#endif
#ifdef USE_KERBEROS5
  | CURL_VERSION_KERBEROS5
#endif
#ifdef HAVE_GSSAPI
  | CURL_VERSION_GSSAPI
#endif
#ifdef USE_WINDOWS_SSPI
  | CURL_VERSION_SSPI
#endif
#ifdef HAVE_LIBZ
  | CURL_VERSION_LIBZ
#endif
#ifdef DEBUGBUILD
  | CURL_VERSION_DEBUG
#endif
#ifdef CURLDEBUG
  | CURL_VERSION_CURLDEBUG
#endif
#ifdef CURLRES_ASYNCH
  | CURL_VERSION_ASYNCHDNS
#endif
#if (SIZEOF_CURL_OFF_T > 4) && \
    ( (SIZEOF_OFF_T > 4) || defined(USE_WIN32_LARGE_FILES) )
  | CURL_VERSION_LARGEFILE
#endif
#if defined(WIN32) && defined(UNICODE) && defined(_UNICODE)
  | CURL_VERSION_UNICODE
#endif
#if defined(CURL_DOES_CONVERSIONS)
  | CURL_VERSION_CONV
#endif
#if defined(USE_TLS_SRP)
  | CURL_VERSION_TLSAUTH_SRP
#endif
#if defined(USE_NGHTTP2) || defined(USE_HYPER)
  | CURL_VERSION_HTTP2
#endif
#if defined(ENABLE_QUIC)
  | CURL_VERSION_HTTP3
#endif
#if defined(USE_UNIX_SOCKETS)
  | CURL_VERSION_UNIX_SOCKETS
#endif
#if defined(USE_LIBPSL)
  | CURL_VERSION_PSL
#endif
#if defined(CURL_WITH_MULTI_SSL)
  | CURL_VERSION_MULTI_SSL
#endif
#if defined(HAVE_BROTLI)
  | CURL_VERSION_BROTLI
#endif
#if defined(HAVE_ZSTD)
  | CURL_VERSION_ZSTD
#endif
#ifndef CURL_DISABLE_ALTSVC
  | CURL_VERSION_ALTSVC
#endif
#ifndef CURL_DISABLE_HSTS
  | CURL_VERSION_HSTS
#endif
#if defined(USE_GSASL)
  | CURL_VERSION_GSASL
#endif
  ,
  NULL, /* ssl_version */
  0,    /* ssl_version_num, this is kept at zero */
  NULL, /* zlib_version */
  protocols,
  NULL, /* c-ares version */
  0,    /* c-ares version numerical */
  NULL, /* libidn version */
  0,    /* iconv version */
  NULL, /* ssh lib version */
  0,    /* brotli_ver_num */
  NULL, /* brotli version */
  0,    /* nghttp2 version number */
  NULL, /* nghttp2 version string */
  NULL, /* quic library string */
#ifdef CURL_CA_BUNDLE
  CURL_CA_BUNDLE, /* cainfo */
#else
  NULL,
#endif
#ifdef CURL_CA_PATH
  CURL_CA_PATH,  /* capath */
#else
  NULL,
#endif
  0,    /* zstd_ver_num */
  NULL, /* zstd version */
  NULL, /* Hyper version */
  NULL  /* gsasl version */
};

    return err;
}

void * __curl_ctx(void)
{
    return &((struct curl_lib_ctx_t *)vsf_linux_library_ctx(__curl_lib_idx))->__curl_ctx;
}

#endif      // __VSF__

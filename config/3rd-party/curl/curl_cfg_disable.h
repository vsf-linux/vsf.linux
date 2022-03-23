// Disable support for Alt-Svc: HTTP headers.
#define CURL_DISABLE_ALTSVC
// Disable support for HTTP cookies.
#define CURL_DISABLE_COOKIES
// Disable support for authentication methods using crypto.
//#define CURL_DISABLE_CRYPTO_AUTH
// Disable the DICT protocol
#define CURL_DISABLE_DICT
// Disable DNS-over-HTTPS
#define CURL_DISABLE_DOH
// Disable the FILE protocol
#define CURL_DISABLE_FILE
// Disable the FTP (and FTPS) protocol
#define CURL_DISABLE_FTP
// Disable the curl_easy_options API calls that lets users get information about existing options to curl_easy_setopt.
#define CURL_DISABLE_GETOPTIONS
// Disable the GOPHER protocol.
#define CURL_DISABLE_GOPHER
// Disable the HTTP Strict Transport Security support.
#define CURL_DISABLE_HSTS
// Disable the HTTP(S) protocols. Note that this then also disable HTTP proxy support.
//#define CURL_DISABLE_HTTP
// Disable support for all HTTP authentication methods.
//#define CURL_DISABLE_HTTP_AUTH
// Disable the IMAP(S) protocols.
#define CURL_DISABLE_IMAP
// Disable the LDAP(S) protocols.
#define CURL_DISABLE_LDAP
// Disable the LDAPS protocol.
#define CURL_DISABLE_LDAPS
// Disable the --libcurl option from the curl tool.
#define CURL_DISABLE_LIBCURL_OPTION
// Disable MIME support.
#define CURL_DISABLE_MIME
// Disable MQTT support.
#define CURL_DISABLE_MQTT
// Disable the netrc parser.
#define CURL_DISABLE_NETRC
// Disable support for NTLM.
#define CURL_DISABLE_NTLM
// Disable the auto load config support in the OpenSSL backend.
#define CURL_DISABLE_OPENSSL_AUTO_LOAD_CONFIG
// Disable date parsing
#define CURL_DISABLE_PARSEDATE
// Disable the POP3 protocol
#define CURL_DISABLE_POP3
// Disable the built-in progress meter
#define CURL_DISABLE_PROGRESS_METER
// Disable support for proxies
#define CURL_DISABLE_PROXY
// Disable the RTSP protocol.
#define CURL_DISABLE_RTSP
// Disable the shuffle DNS feature
#define CURL_DISABLE_SHUFFLE_DNS
// Disable the SMB(S) protocols
#define CURL_DISABLE_SMB
// Disable the SMTP(S) protocols
#define CURL_DISABLE_SMTP
// Disable the use of socketpair internally to allow waking up and canceling curl_multi_poll().
#define CURL_DISABLE_SOCKETPAIR
// Disable the TELNET protocol
#define CURL_DISABLE_TELNET
// Disable the TFTP protocol
#define CURL_DISABLE_TFTP
// Disable verbose strings and error messages.
#define CURL_DISABLE_VERBOSE_STRINGS
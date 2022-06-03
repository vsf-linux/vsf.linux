#undef _WIN32
#undef _MSC_VER

#define VERSION                     "0.11.0+dev"
#define RELSTR                      "00698"
#define GITVERSION                  VERSION "-" RELSTR "-g" "cc8b49185 (" __DATE__ ")"

#define PACKAGE                     "openocd"
#define PACKAGE_BUGREPORT           "OpenOCD Mailing List <openocd-devel@lists.sourceforge.net>"
#define PACKAGE_NAME                PACKAGE
#define PACKAGE_STRING              PACKAGE " " VERSION
#define PACKAGE_URL                 ""
#define PACKAGE_VERSION             VERSION
#define BINDIR                      "/bin"
#define PKGDATADIR                  "123"

#define STDC_HEADERS                1
#define _GNU_SOURCE                 1

// adapters
#define BUILD_CMSIS_DAP_USB         1

#define HAVE_LONG_LONG_INT          1
#define HAVE_UNSIGNED_LONG_LONG_INT 1
#define IS_WIN32                    0

#define HAVE_DIRENT_H 1
#define HAVE_FCNTL_H 1
#define HAVE_IFADDRS_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_LIBUSB1 1
#define HAVE_NETDB_H 1
#define HAVE_NETINET_IN_H 1
#define HAVE_NETINET_TCP_H 1
#define HAVE_NET_IF_H 1
#define HAVE_POLL_H 1
#define HAVE_PTHREAD_H 1
#define HAVE_REALPATH 1
#define HAVE_STDBOOL_H 1
#define HAVE_STDINT_H 1
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_SYS_IOCTL_H 1
#define HAVE_SYS_SELECT_H 1
#define HAVE_SYS_SOCKET_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TIME_H 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_UNISTD_H 1

#define HAVE_GETTIMEOFDAY 1
#define HAVE_USLEEP 1
#define HAVE_VASPRINTF 1
#define HAVE_STRNLEN 1
#undef HAVE_STRNDUP

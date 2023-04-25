#ifdef __VSF_APPLET__
"\
#undef _WIN32\n\
#undef _WIN64\n\
"
#endif
"\
#define __VSF__\n\
#define __CPU_X64__\n\
#define __WIN__\n\
#include <tccdefs.h>\n\
"
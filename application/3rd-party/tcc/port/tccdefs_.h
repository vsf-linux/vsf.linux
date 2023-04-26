"\
#define __VSF__\n\
#define __VSF_APPLET__\n\
#define __CPU_X64__\n\
#define __WIN__\n\
#include <tccdefs.h>\n\
"

#ifdef __VSF_APPLET__
// IMPORT: undef _WIN32 and _WIN64 after tccdefs.h
"\
#undef _WIN32\n\
#undef _WIN64\n\
#include <vsf_cfg.h>\n\
"
#endif

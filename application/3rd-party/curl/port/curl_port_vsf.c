#ifdef __VSF__

// for puts only
#include "3rd-party/curl/raw/src/tool_setup.h"

int vsf_curl_dynlib_idx = -1;

void hugehelp(void)
{
  puts("This is a silly replacement for the actual file.");
}

#endif      // __VSF__

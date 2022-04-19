#ifdef __WIN__
#include <Windows.h>

int os_thread_env_init()
{
    return 0;
}

void os_thread_env_destroy()
{
}

void os_dcache_flush(void)
{
}

unsigned os_getpagesize()
{
    SYSTEM_INFO sys_info;
    GetNativeSystemInfo(&sys_info);
    return (unsigned)sys_info.dwPageSize;
}
#endif

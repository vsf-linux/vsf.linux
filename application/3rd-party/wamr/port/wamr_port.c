#if defined(__WIN__)
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
#elif defined(__LINUX__)
// TODO
#else
#include "platform_api_vmcore.h"

void * os_mmap(void *hint, size_t size, int prot, int flags)
{
    return malloc(size);
}

void os_munmap(void *addr, size_t size)
{
    free(addr);
}

int os_mprotect(void *addr, size_t size, int prot)
{
    return 0;
}

void os_dcache_flush(void)
{
}
#endif

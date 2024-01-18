#ifndef __BUSYBOX_PORT_VSF_H__
#define __BUSYBOX_PORT_VSF_H__

#define __vsf_busybox_mod_name(__mod_name)    vsf ## _ ## __mod_name
#define vsf_busybox_mod_name(__mod_name)      __vsf_busybox_mod_name(__mod_name)
#define declare_vsf_busybox_mod(__mod_name)                                     \
        extern const vsf_linux_dynlib_mod_t vsf_busybox_mod_name(__mod_name);
enum {
    VSF_BUSYBOX_MOD_LIBBB = 0,
};

#endif      // __BUSYBOX_PORT_VSF_H__

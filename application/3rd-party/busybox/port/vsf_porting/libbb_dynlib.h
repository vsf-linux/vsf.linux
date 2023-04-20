#ifndef __BUSYBOX_PORT_VSF_H__
#define __BUSYBOX_PORT_VSF_H__

#define __vsf_busybox_mod_name(__mod_name)    vsf ## _ ## __mod_name
#define vsf_busybox_mod_name(__mod_name)      __vsf_busybox_mod_name(__mod_name)
#define declare_vsf_busybox_mod(__mod_name)                                     \
        extern const vsf_linux_dynlib_mod_t vsf_busybox_mod_name(__mod_name);
#define define_vsf_busybox_mod(__mod_name, __mod_size, __mod_idx, __mod_init)   \
        const vsf_linux_dynlib_mod_t vsf_busybox_mod_name(__mod_name) = {       \
            .lib_idx            = &vsf_busybox_dynlib_idx,                      \
            .mod_idx            = (__mod_idx),                                  \
            .module_num         = VSF_BUSYBOX_MOD_NUM,                          \
            .bss_size           = VSF_BUSYBOX_BSS_SIZE,                         \
            .mod_size           = (__mod_size),                                 \
            .init               = (__mod_init),                                 \
        };
#define VSF_BUSYBOX_BSS_SIZE      32768
enum {
    VSF_BUSYBOX_MOD_LIBBB = 0,
    VSF_BUSYBOX_MOD_NUM,
};
extern int vsf_busybox_dynlib_idx;

#endif      // __BUSYBOX_PORT_VSF_H__

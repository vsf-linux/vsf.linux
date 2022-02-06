#ifndef __CURL_PORT_VSF_H__
#define __CURL_PORT_VSF_H__

#define __vsf_curl_mod_name(__mod_name)     vsf ## _ ## __mod_name
#define vsf_curl_mod_name(__mod_name)       __vsf_curl_mod_name(__mod_name)
#define declare_vsf_curl_mod(__mod_name)                                        \
        extern const vsf_linux_dynlib_mod_t vsf_curl_mod_name(__mod_name);
#define define_vsf_curl_mod(__mod_name, __mod_size, __mod_idx, __mod_init)      \
        const vsf_linux_dynlib_mod_t vsf_curl_mod_name(__mod_name) = {          \
            .lib_idx            = &vsf_curl_dynlib_idx,                         \
            .mod_idx            = (__mod_idx),                                  \
            .module_num         = VSF_CURL_MOD_NUM,                             \
            .bss_size           = VSF_CURL_BSS_SIZE,                            \
            .mod_size           = (__mod_size),                                 \
            .init               = (__mod_init),                                 \
        };
#define VSF_CURL_BSS_SIZE       32768
enum {
    VSF_CURL_MOD_TOOL_CB_DBG = 0,
    VSF_CURL_MOD_TOOL_CONVERT,
    VSF_CURL_MOD_TOOL_GETPASS,
    VSF_CURL_MOD_TOOL_OPERATE,
    VSF_CURL_MOD_TOOL_PARAMHLP,
    VSF_CURL_MOD_TOOL_PROGRESS,
    VSF_CURL_MOD_NUM,
};
extern int vsf_curl_dynlib_idx;

#endif      // __CURL_PORT_VSF_H__

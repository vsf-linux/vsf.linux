#ifndef __LESS_PORT_VSF_H__
#define __LESS_PORT_VSF_H__

#define __vsf_less_mod_name(__mod_name)     vsf ## _ ## __mod_name
#define vsf_less_mod_name(__mod_name)       __vsf_less_mod_name(__mod_name)
#define declare_vsf_less_mod(__mod_name)                                        \
        extern const vsf_linux_dynlib_mod_t vsf_less_mod_name(__mod_name);
#define define_vsf_less_mod(__mod_name, __mod_size, __mod_idx, __mod_init)      \
        const vsf_linux_dynlib_mod_t vsf_less_mod_name(__mod_name) = {          \
            .lib_idx            = &vsf_less_dynlib_idx,                         \
            .mod_idx            = (__mod_idx),                                  \
            .module_num         = VSF_LESS_MOD_NUM,                             \
            .bss_size           = VSF_LESS_BSS_SIZE,                            \
            .mod_size           = (__mod_size),                                 \
            .init               = (__mod_init),                                 \
        };
#define VSF_LESS_BSS_SIZE       32768
enum {
    VSF_LESS_MOD_PUBLIC = 0,
    VSF_LESS_MOD_CH,
    VSF_LESS_MOD_CHARSET,
    VSF_LESS_MOD_CMDBUG,
    VSF_LESS_MOD_COMMAND,
    VSF_LESS_MOD_DECODE,
    VSF_LESS_MOD_FILENAME,
#if HAVE_TIME
    VSF_LESS_MOD_FORWBACK,
#endif
    VSF_LESS_MOD_IFILE,
    VSF_LESS_MOD_LINE,
    VSF_LESS_MOD_LINENUM,
    VSF_LESS_MOD_MAIN,
    VSF_LESS_MOD_MARK,
    VSF_LESS_MOD_OPTION,
    VSF_LESS_MOD_OPTTBL,
    VSF_LESS_MOD_OS,
    VSF_LESS_MOD_OUTPUT,
    VSF_LESS_MOD_POSITION,
    VSF_LESS_MOD_PROMPT,
    VSF_LESS_MOD_REGEXP,
    VSF_LESS_MOD_SCREEN,
    VSF_LESS_MOD_SEARCH,
#if TAGS
    VSF_LESS_MOD_TAGS,
#endif

    VSF_LESS_MOD_NUM,
};
extern int vsf_less_dynlib_idx;

#endif      // __LESS_PORT_VSF_H__

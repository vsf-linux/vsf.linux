/*
 * Copyright (C) 2019 Intel Corporation.  All rights reserved.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <string.h>

#include "bh_platform.h"
#include "bh_read_file.h"
#include "wasm_export.h"

static int app_argc;
static char **app_argv;

/* clang-format off */
static int
print_help()
{
    printf("Usage: iwasm [-options] wasm_file [args...]\n");
    printf("options:\n");
    printf("  -f|--function name     Specify a function name of the module to run rather\n"
           "                         than main\n");
#if WASM_ENABLE_LOG != 0
    printf("  -v=n                   Set log verbose level (0 to 5, default is 2) larger\n"
           "                         level with more log\n");
#endif
    printf("  --stack-size=n         Set maximum stack size in bytes, default is 16 KB\n");
    printf("  --heap-size=n          Set maximum heap size in bytes, default is 16 KB\n");
    printf("  --repl                 Start a very simple REPL (read-eval-print-loop) mode\n"
           "                         that runs commands in the form of \"FUNC ARG...\"\n");
#if WASM_ENABLE_LIBC_WASI != 0
    printf("  --env=<env>            Pass wasi environment variables with \"key=value\"\n");
    printf("                         to the program, for example:\n");
    printf("                           --env=\"key1=value1\" --env=\"key2=value2\"\n");
    printf("  --dir=<dir>            Grant wasi access to the given host directories\n");
    printf("                         to the program, for example:\n");
    printf("                           --dir=<dir1> --dir=<dir2>\n");
    printf("  --addr-pool=           Grant wasi access to the given network addresses in\n");
    printf("                         CIRD notation to the program, seperated with ',',\n");
    printf("                         for example:\n");
    printf("                           --addr-pool=1.2.3.4/15,2.3.4.5/16\n");
#endif
#if WASM_ENABLE_MULTI_MODULE != 0
    printf("  --module-path=         Indicate a module search path. default is current\n"
           "                         directory('./')\n");
#endif
#if WASM_ENABLE_LIB_PTHREAD != 0
    printf("  --max-threads=n        Set maximum thread number per cluster, default is 4\n");
#endif
#if WASM_ENABLE_DEBUG_INTERP != 0
    printf("  -g=ip:port             Set the debug sever address, default is debug disabled\n");
    printf("                           if port is 0, then a random port will be used\n");
#endif
    return 1;
}
/* clang-format on */

// patch some wamr APIs to support WASM_ENABLE_LIBC_VSFWASI
#if WASM_ENABLE_INTERP != 0
#include "wasm_runtime.h"
#endif
#if WASM_ENABLE_AOT != 0
#include "aot_runtime.h"
#endif

#if WASM_ENABLE_LIBC_WASI == 0
WASMFunctionInstanceCommon *
wasm_runtime_lookup_wasi_start_function(WASMModuleInstanceCommon *module_inst)
{
    uint32 i;

#if WASM_ENABLE_INTERP != 0
    if (module_inst->module_type == Wasm_Module_Bytecode) {
        WASMModuleInstance *wasm_inst = (WASMModuleInstance *)module_inst;
        WASMFunctionInstance *func;
        for (i = 0; i < wasm_inst->export_func_count; i++) {
            if (!strcmp(wasm_inst->export_functions[i].name, "_start")) {
                func = wasm_inst->export_functions[i].function;
                if (func->u.func->func_type->param_count != 0
                    || func->u.func->func_type->result_count != 0) {
                    LOG_ERROR("Lookup wasi _start function failed: "
                              "invalid function type.\n");
                    return NULL;
                }
                return (WASMFunctionInstanceCommon *)func;
            }
        }
        return NULL;
    }
#endif

#if WASM_ENABLE_AOT != 0
    if (module_inst->module_type == Wasm_Module_AoT) {
        AOTModuleInstance *aot_inst = (AOTModuleInstance *)module_inst;
        AOTFunctionInstance *export_funcs =
            (AOTFunctionInstance *)aot_inst->export_funcs.ptr;
        for (i = 0; i < aot_inst->export_func_count; i++) {
            if (!strcmp(export_funcs[i].func_name, "_start")) {
                AOTFuncType *func_type = export_funcs[i].u.func.func_type;
                if (func_type->param_count != 0
                    || func_type->result_count != 0) {
                    LOG_ERROR("Lookup wasi _start function failed: "
                              "invalid function type.\n");
                    return NULL;
                }
                return (WASMFunctionInstanceCommon *)&export_funcs[i];
            }
        }
        return NULL;
    }
#endif /* end of WASM_ENABLE_AOT */

    return NULL;
}
#endif

/**
 * Implementation of wasm_application_execute_main()
 */
static bool
check_main_func_type(const WASMType *type)
{
    if (!(type->param_count == 0 || type->param_count == 2)
        || type->result_count > 1) {
        LOG_ERROR(
            "WASM execute application failed: invalid main function type.\n");
        return false;
    }

    if (type->param_count == 2
        && !(type->types[0] == VALUE_TYPE_I32
             && type->types[1] == VALUE_TYPE_I32)) {
        LOG_ERROR(
            "WASM execute application failed: invalid main function type.\n");
        return false;
    }

    if (type->result_count
        && type->types[type->param_count] != VALUE_TYPE_I32) {
        LOG_ERROR(
            "WASM execute application failed: invalid main function type.\n");
        return false;
    }

    return true;
}

bool
wasm_application_execute_main_app(WASMModuleInstanceCommon *module_inst, int32 argc,
                              char *argv[])
{
    WASMFunctionInstanceCommon *func;
    WASMType *func_type = NULL;
    WASMExecEnv *exec_env = NULL;
    uint32 argc1 = 0, argv1[2] = { 0 };
    uint32 total_argv_size = 0;
    uint64 total_size;
    uint32 argv_buf_offset = 0;
    int32 i;
    char *argv_buf, *p, *p_end;
    uint32 *argv_offsets, module_type;
    bool ret, is_import_func = true;

    exec_env = wasm_runtime_get_exec_env_singleton(module_inst);
    if (!exec_env) {
        wasm_runtime_set_exception(module_inst,
                                   "create singleton exec_env failed");
        return false;
    }

#if WASM_ENABLE_LIBC_WASI != 0 || WASM_ENABLE_LIBC_VSFWASI != 0
    /* In wasi mode, we should call the function named "_start"
       which initializes the wasi envrionment and then calls
       the actual main function. Directly calling main function
       may cause exception thrown. */
    if ((func = wasm_runtime_lookup_wasi_start_function(module_inst))) {
#if WASM_ENABLE_DEBUG_INTERP != 0
        wasm_runtime_start_debug_instance(exec_env);
#endif
        return wasm_runtime_call_wasm(exec_env, func, 0, NULL);
    }
#endif /* end of WASM_ENABLE_LIBC_WASI */

    if (!(func = wasm_runtime_lookup_function(module_inst, "main", NULL))
        && !(func = wasm_runtime_lookup_function(module_inst,
                                                 "__main_argc_argv", NULL))
        && !(func = wasm_runtime_lookup_function(module_inst, "_main", NULL))) {
#if WASM_ENABLE_LIBC_WASI != 0 || WASM_ENABLE_LIBC_VSFWASI != 0
        wasm_runtime_set_exception(
            module_inst, "lookup the entry point symbol (like _start, main, "
                         "_main, __main_argc_argv) failed");
#else
        wasm_runtime_set_exception(module_inst,
                                   "lookup the entry point symbol (like main, "
                                   "_main, __main_argc_argv) failed");
#endif
        return false;
    }

#if WASM_ENABLE_INTERP != 0
    if (module_inst->module_type == Wasm_Module_Bytecode) {
        is_import_func = ((WASMFunctionInstance *)func)->is_import_func;
    }
#endif
#if WASM_ENABLE_AOT != 0
    if (module_inst->module_type == Wasm_Module_AoT) {
        is_import_func = ((AOTFunctionInstance *)func)->is_import_func;
    }
#endif

    if (is_import_func) {
        wasm_runtime_set_exception(module_inst, "lookup main function failed");
        return false;
    }

    module_type = module_inst->module_type;
    func_type = wasm_runtime_get_function_type(func, module_type);

    if (!func_type) {
        LOG_ERROR("invalid module instance type");
        return false;
    }

    if (!check_main_func_type(func_type)) {
        wasm_runtime_set_exception(module_inst,
                                   "invalid function type of main function");
        return false;
    }

    if (func_type->param_count) {
        for (i = 0; i < argc; i++)
            total_argv_size += (uint32)(strlen(argv[i]) + 1);
        total_argv_size = align_uint(total_argv_size, 4);

        total_size = (uint64)total_argv_size + sizeof(int32) * (uint64)argc;

        if (total_size >= UINT32_MAX
            || !(argv_buf_offset = wasm_runtime_module_malloc(
                     module_inst, (uint32)total_size, (void **)&argv_buf))) {
            wasm_runtime_set_exception(module_inst, "allocate memory failed");
            return false;
        }

        p = argv_buf;
        argv_offsets = (uint32 *)(p + total_argv_size);
        p_end = p + total_size;

        for (i = 0; i < argc; i++) {
            bh_memcpy_s(p, (uint32)(p_end - p), argv[i],
                        (uint32)(strlen(argv[i]) + 1));
            argv_offsets[i] = argv_buf_offset + (uint32)(p - argv_buf);
            p += strlen(argv[i]) + 1;
        }

        argc1 = 2;
        argv1[0] = (uint32)argc;
        argv1[1] =
            (uint32)wasm_runtime_addr_native_to_app(module_inst, argv_offsets);
    }

#if WASM_ENABLE_DEBUG_INTERP != 0
    wasm_runtime_start_debug_instance(exec_env);
#endif

    ret = wasm_runtime_call_wasm(exec_env, func, argc1, argv1);
    if (ret && func_type->result_count > 0 && argc > 0 && argv)
        /* copy the return value */
        *(int *)argv = (int)argv1[0];

    if (argv_buf_offset)
        wasm_runtime_module_free(module_inst, argv_buf_offset);
    return ret;
}

// end of patch wamr APIs



static void *
app_instance_main(wasm_module_inst_t module_inst)
{
    const char *exception;

    wasm_application_execute_main_app(module_inst, app_argc, app_argv);
    if ((exception = wasm_runtime_get_exception(module_inst)))
        printf("%s\n", exception);
    return NULL;
}

static void *
app_instance_func(wasm_module_inst_t module_inst, const char *func_name)
{
    wasm_application_execute_func(module_inst, func_name, app_argc - 1,
                                  app_argv + 1);
    /* The result of wasm function or exception info was output inside
       wasm_application_execute_func(), here we don't output them again. */
    return NULL;
}

/**
 * Split a space separated strings into an array of strings
 * Returns NULL on failure
 * Memory must be freed by caller
 * Based on: http://stackoverflow.com/a/11198630/471795
 */
static char **
split_string(char *str, int *count)
{
    char **res = NULL;
    char *p;
    int idx = 0;

    /* split string and append tokens to 'res' */
    do {
        p = strtok(str, " ");
        str = NULL;
        res = (char **)realloc(res, sizeof(char *) * (uint32)(idx + 1));
        if (res == NULL) {
            return NULL;
        }
        res[idx++] = p;
    } while (p);

    /**
     * since the function name,
     * res[0] might be contains a '\' to indicate a space
     * func\name -> func name
     */
    p = strchr(res[0], '\\');
    while (p) {
        *p = ' ';
        p = strchr(p, '\\');
    }

    if (count) {
        *count = idx - 1;
    }
    return res;
}

static void *
app_instance_repl(wasm_module_inst_t module_inst)
{
    char *cmd = NULL;
    size_t len = 0;
    ssize_t n;

    while ((printf("webassembly> "), n = getline(&cmd, &len, stdin)) != -1) {
        bh_assert(n > 0);
        if (cmd[n - 1] == '\n') {
            if (n == 1)
                continue;
            else
                cmd[n - 1] = '\0';
        }
        if (!strcmp(cmd, "__exit__")) {
            printf("exit repl mode\n");
            break;
        }
        app_argv = split_string(cmd, &app_argc);
        if (app_argv == NULL) {
            LOG_ERROR("Wasm prepare param failed: split string failed.\n");
            break;
        }
        if (app_argc != 0) {
            wasm_application_execute_func(module_inst, app_argv[0],
                                          app_argc - 1, app_argv + 1);
        }
        free(app_argv);
    }
    free(cmd);
    return NULL;
}

#if WASM_ENABLE_LIBC_WASI != 0
static bool
validate_env_str(char *env)
{
    char *p = env;
    int key_len = 0;

    while (*p != '\0' && *p != '=') {
        key_len++;
        p++;
    }

    if (*p != '=' || key_len == 0)
        return false;

    return true;
}
#endif

#if WASM_ENABLE_GLOBAL_HEAP_POOL != 0
#ifdef __NuttX__
static char global_heap_buf[WASM_GLOBAL_HEAP_SIZE * BH_KB] = { 0 };
#else
static char global_heap_buf[10 * 1024 * 1024] = { 0 };
#endif
#endif

#if WASM_ENABLE_MULTI_MODULE != 0
static char *
handle_module_path(const char *module_path)
{
    /* next character after = */
    return (strchr(module_path, '=')) + 1;
}

static char *module_search_path = ".";

static bool
module_reader_callback(const char *module_name, uint8 **p_buffer,
                       uint32 *p_size)
{
    const char *format = "%s/%s.wasm";
    int sz = strlen(module_search_path) + strlen("/") + strlen(module_name)
             + strlen(".wasm") + 1;
    char *wasm_file_name = BH_MALLOC(sz);
    if (!wasm_file_name) {
        return false;
    }

    snprintf(wasm_file_name, sz, format, module_search_path, module_name);

    *p_buffer = (uint8_t *)bh_read_file_to_buffer(wasm_file_name, p_size);

    wasm_runtime_free(wasm_file_name);
    return *p_buffer != NULL;
}

static void
moudle_destroyer(uint8 *buffer, uint32 size)
{
    if (!buffer) {
        return;
    }

    wasm_runtime_free(buffer);
    buffer = NULL;
}
#endif /* WASM_ENABLE_MULTI_MODULE */

int
main(int argc, char *argv[])
{
    char *wasm_file = NULL;
    const char *func_name = NULL;
    uint8 *wasm_file_buf = NULL;
    uint32 wasm_file_size;
    uint32 stack_size = 16 * 1024, heap_size = 16 * 1024;
    wasm_module_t wasm_module = NULL;
    wasm_module_inst_t wasm_module_inst = NULL;
    RuntimeInitArgs init_args;
    char error_buf[128] = { 0 };
#if WASM_ENABLE_LOG != 0
    int log_verbose_level = 2;
#endif
    bool is_repl_mode = false;
    bool is_xip_file = false;
#if WASM_ENABLE_LIBC_WASI != 0
    const char *dir_list[8] = { NULL };
    uint32 dir_list_size = 0;
    const char *env_list[8] = { NULL };
    uint32 env_list_size = 0;
    const char *addr_pool[8] = { NULL };
    uint32 addr_pool_size = 0;
#endif
#if WASM_ENABLE_DEBUG_INTERP != 0
    char *ip_addr = NULL;
    /* int platform_port = 0; */
    int instance_port = 0;
#endif

    /* Process options. */
    for (argc--, argv++; argc > 0 && argv[0][0] == '-'; argc--, argv++) {
        if (!strcmp(argv[0], "-f") || !strcmp(argv[0], "--function")) {
            argc--, argv++;
            if (argc < 2) {
                print_help();
                return 0;
            }
            func_name = argv[0];
        }
#if WASM_ENABLE_LOG != 0
        else if (!strncmp(argv[0], "-v=", 3)) {
            log_verbose_level = atoi(argv[0] + 3);
            if (log_verbose_level < 0 || log_verbose_level > 5)
                return print_help();
        }
#endif
        else if (!strcmp(argv[0], "--repl")) {
            is_repl_mode = true;
        }
        else if (!strncmp(argv[0], "--stack-size=", 13)) {
            if (argv[0][13] == '\0')
                return print_help();
            stack_size = atoi(argv[0] + 13);
        }
        else if (!strncmp(argv[0], "--heap-size=", 12)) {
            if (argv[0][12] == '\0')
                return print_help();
            heap_size = atoi(argv[0] + 12);
        }
#if WASM_ENABLE_LIBC_WASI != 0
        else if (!strncmp(argv[0], "--dir=", 6)) {
            if (argv[0][6] == '\0')
                return print_help();
            if (dir_list_size >= sizeof(dir_list) / sizeof(char *)) {
                printf("Only allow max dir number %d\n",
                       (int)(sizeof(dir_list) / sizeof(char *)));
                return -1;
            }
            dir_list[dir_list_size++] = argv[0] + 6;
        }
        else if (!strncmp(argv[0], "--env=", 6)) {
            char *tmp_env;

            if (argv[0][6] == '\0')
                return print_help();
            if (env_list_size >= sizeof(env_list) / sizeof(char *)) {
                printf("Only allow max env number %d\n",
                       (int)(sizeof(env_list) / sizeof(char *)));
                return -1;
            }
            tmp_env = argv[0] + 6;
            if (validate_env_str(tmp_env))
                env_list[env_list_size++] = tmp_env;
            else {
                printf("Wasm parse env string failed: expect \"key=value\", "
                       "got \"%s\"\n",
                       tmp_env);
                return print_help();
            }
        }
        /* TODO: parse the configuration file via --addr-pool-file */
        else if (!strncmp(argv[0], "--addr-pool=", strlen("--addr-pool="))) {
            /* like: --addr-pool=100.200.244.255/30 */
            char *token = NULL;

            if ('\0' == argv[0][12])
                return print_help();

            token = strtok(argv[0] + strlen("--addr-pool="), ",");
            while (token) {
                if (addr_pool_size >= sizeof(addr_pool) / sizeof(char *)) {
                    printf("Only allow max address number %d\n",
                           (int)(sizeof(addr_pool) / sizeof(char *)));
                    return -1;
                }

                addr_pool[addr_pool_size++] = token;
                token = strtok(NULL, ";");
            }
        }
#endif /* WASM_ENABLE_LIBC_WASI */
#if WASM_ENABLE_MULTI_MODULE != 0
        else if (!strncmp(argv[0],
                          "--module-path=", strlen("--module-path="))) {
            module_search_path = handle_module_path(argv[0]);
            if (!strlen(module_search_path)) {
                return print_help();
            }
        }
#endif
#if WASM_ENABLE_LIB_PTHREAD != 0
        else if (!strncmp(argv[0], "--max-threads=", 14)) {
            if (argv[0][14] == '\0')
                return print_help();
            wasm_runtime_set_max_thread_num(atoi(argv[0] + 14));
        }
#endif
#if WASM_ENABLE_DEBUG_INTERP != 0
        else if (!strncmp(argv[0], "-g=", 3)) {
            char *port_str = strchr(argv[0] + 3, ':');
            char *port_end;
            if (port_str == NULL)
                return print_help();
            *port_str = '\0';
            instance_port = strtoul(port_str + 1, &port_end, 10);
            if (port_str[1] == '\0' || *port_end != '\0')
                return print_help();
            ip_addr = argv[0] + 3;
        }
#endif
        else
            return print_help();
    }

    if (argc == 0)
        return print_help();

    wasm_file = argv[0];
    app_argc = argc;
    app_argv = argv;

    memset(&init_args, 0, sizeof(RuntimeInitArgs));

#if WASM_ENABLE_GLOBAL_HEAP_POOL != 0
    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);
#else
    init_args.mem_alloc_type = Alloc_With_Allocator;
    init_args.mem_alloc_option.allocator.malloc_func = malloc;
    init_args.mem_alloc_option.allocator.realloc_func = realloc;
    init_args.mem_alloc_option.allocator.free_func = free;
#endif

#if WASM_ENABLE_DEBUG_INTERP != 0
    init_args.platform_port = 0;
    init_args.instance_port = instance_port;
    if (ip_addr)
        strcpy(init_args.ip_addr, ip_addr);
#endif

#if WASM_ENABLE_LIBC_VSFWASI != 0
    init_args.native_module_name = "wasi_snapshot_preview1";
    init_args.n_native_symbols = get_libc_wasi_export_apis(&init_args.native_symbols);
#endif

    /* initialize runtime environment */
    if (!wasm_runtime_full_init(&init_args)) {
        printf("Init runtime environment failed.\n");
        return -1;
    }

#if WASM_ENABLE_LOG != 0
    bh_log_set_verbose_level(log_verbose_level);
#endif

    /* load WASM byte buffer from WASM bin file */
    if (!(wasm_file_buf =
              (uint8 *)bh_read_file_to_buffer(wasm_file, &wasm_file_size)))
        goto fail1;

#if WASM_ENABLE_AOT != 0
    if (wasm_runtime_is_xip_file(wasm_file_buf, wasm_file_size)) {
        uint8 *wasm_file_mapped;
        int map_prot = MMAP_PROT_READ | MMAP_PROT_WRITE | MMAP_PROT_EXEC;
        int map_flags = MMAP_MAP_32BIT;

        if (!(wasm_file_mapped =
                  os_mmap(NULL, (uint32)wasm_file_size, map_prot, map_flags))) {
            printf("mmap memory failed\n");
            wasm_runtime_free(wasm_file_buf);
            goto fail1;
        }

        bh_memcpy_s(wasm_file_mapped, wasm_file_size, wasm_file_buf,
                    wasm_file_size);
        wasm_runtime_free(wasm_file_buf);
        wasm_file_buf = wasm_file_mapped;
        is_xip_file = true;
    }
#endif

#if WASM_ENABLE_MULTI_MODULE != 0
    wasm_runtime_set_module_reader(module_reader_callback, moudle_destroyer);
#endif

    /* load WASM module */
    if (!(wasm_module = wasm_runtime_load(wasm_file_buf, wasm_file_size,
                                          error_buf, sizeof(error_buf)))) {
        printf("%s\n", error_buf);
        goto fail2;
    }

#if WASM_ENABLE_LIBC_WASI != 0
    wasm_runtime_set_wasi_args(wasm_module, dir_list, dir_list_size, NULL, 0,
                               env_list, env_list_size, argv, argc);

    wasm_runtime_set_wasi_addr_pool(wasm_module, addr_pool, addr_pool_size);
#endif
#if WASM_ENABLE_LIBC_VSFWASI != 0
    extern int wasm_runtime_set_vsfwasi_args(wasm_module_t module, char *argv[], int argc);
    if (wasm_runtime_set_vsfwasi_args(wasm_module, argv, argc) < 0) {
        printf("fail to set vsfwasi arguments\n");
        goto fail2;
    }
#endif

    /* instantiate the module */
    if (!(wasm_module_inst =
              wasm_runtime_instantiate(wasm_module, stack_size, heap_size,
                                       error_buf, sizeof(error_buf)))) {
        printf("%s\n", error_buf);
        goto fail3;
    }

    if (is_repl_mode)
        app_instance_repl(wasm_module_inst);
    else if (func_name)
        app_instance_func(wasm_module_inst, func_name);
    else
        app_instance_main(wasm_module_inst);

    /* destroy the module instance */
    wasm_runtime_deinstantiate(wasm_module_inst);

fail3:
    /* unload the module */
    wasm_runtime_unload(wasm_module);

fail2:
    /* free the file buffer */
    if (!is_xip_file)
        wasm_runtime_free(wasm_file_buf);
    else
        os_munmap(wasm_file_buf, wasm_file_size);

fail1:
    /* destroy runtime environment */
    wasm_runtime_destroy();
    return 0;
}

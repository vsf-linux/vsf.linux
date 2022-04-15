#include "vsf.h"
#if APP_USE_WAMR_DEMO == ENABLED

#include "wasm_export.h"
#include "aot_export.h"

int wamr_run_main(int argc, char *argv[])
{
    uint8_t *wasm_file_buf = NULL;
    uint32_t wasm_file_size;
    wasm_module_t wasm_module = NULL;
    wasm_module_inst_t wasm_module_inst = NULL;
    RuntimeInitArgs init_args;
    char error_buf[128];
#if WASM_ENABLE_LOG != 0
    int log_verbose_level = 2;
#endif

    if (argc < 2) {
        printf("format: %s wasm_file|aot_file [APP_ARG...]\n", argv[0]);
        return -1;
    }

    FILE *f = fopen(argv[1], "rb");
    if (NULL == f) {
        printf("fail to open %s\n", argv[1]);
        return -1;
    }
    fseek(f, 0, SEEK_END);
    wasm_file_size = ftell(f);
    rewind(f);

    wasm_file_buf = malloc(wasm_file_size);
    if (!wasm_file_buf) {
        printf("fail to allocate file buffer\n");
        goto fail0;
    }
    if (wasm_file_size != fread(wasm_file_buf, 1, wasm_file_size, f)) {
        printf("fail to read %s\n", argv[1]);
        goto fail0;
    }

    memset(&init_args, 0, sizeof(RuntimeInitArgs));

    static char global_heap_buf[256 * 1024] = { 0 };
    init_args.mem_alloc_type = Alloc_With_Pool;
    init_args.mem_alloc_option.pool.heap_buf = global_heap_buf;
    init_args.mem_alloc_option.pool.heap_size = sizeof(global_heap_buf);

    /* initialize runtime environment */
    if (!wasm_runtime_full_init(&init_args)) {
        printf("Init runtime environment failed.\n");
        goto fail0;
    }

#if WASM_ENABLE_LOG != 0
    bh_log_set_verbose_level(log_verbose_level);
#endif

    /* load WASM module */
    if (!(wasm_module = wasm_runtime_load(wasm_file_buf, wasm_file_size,
                                          error_buf, sizeof(error_buf)))) {
        printf("%s\n", error_buf);
        goto fail1;
    }

    /* instantiate the module */
    if (!(wasm_module_inst = wasm_runtime_instantiate(
              wasm_module, 8 * 1024, 8 * 1024, error_buf, sizeof(error_buf)))) {
        printf("%s\n", error_buf);
        goto fail2;
    }

    {
        const char *exception;

        wasm_application_execute_main(wasm_module_inst, argc - 2, argv + 2);
        if ((exception = wasm_runtime_get_exception(wasm_module_inst)))
            printf("%s\n", exception);
    }

    /* destroy the module instance */
    wasm_runtime_deinstantiate(wasm_module_inst);

fail2:
    /* unload the module */
    wasm_runtime_unload(wasm_module);

fail1:
    /* destroy runtime environment */
    wasm_runtime_destroy();
fail0:
    if (wasm_file_buf != NULL) {
        free(wasm_file_buf);
    }
    if (f != NULL) {
        fclose(f);
    }
    return 0;
}
#endif

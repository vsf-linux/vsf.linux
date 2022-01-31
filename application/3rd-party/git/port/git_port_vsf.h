struct git_ctx_t {
    int dummy;
};

void * __git_ctx(void);
#define git_ctx                 ((struct git_ctx_t *)__git_ctx())

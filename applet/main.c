#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

void *thread_entry(void *param)
{
    printf("hello world\n");
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t id;
    if (pthread_create(&id, NULL, thread_entry, NULL)) {
        printf("fail to create thread\n");
        return -1;
    }
    pthread_join(id, NULL);
    return 0;
}

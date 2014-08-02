#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

void sleep_sort(unsigned int * arr, int size)
{
    pthread_mutex_t lock;
    int i;
    int j = 0;
    unsigned int * out = malloc(sizeof(unsigned int) * size);
    pthread_t * threads = malloc(sizeof(pthread_t) * size);

    /* Nested function to allow access to the mutex. This will only work
     * with gcc. I could do this better, but I'm lazy. */
    void * do_sleep(void * ptr)
    {
        unsigned int val = *(unsigned int*)ptr;
        usleep(val * 10000);

        pthread_mutex_lock(&lock);
        out[j++] = val;
        pthread_mutex_unlock(&lock);

        return NULL;
    }

    pthread_mutex_init(&lock, NULL);
    /* Spawn the sleepers */
    for (i = 0; i < size; ++i)
    {
        pthread_create(&threads[i], NULL, do_sleep, &arr[i]);
    }

    /* Wait for the sleepers */
    for (i = 0; i < size; ++i)
    {
        void ** dc;
        pthread_join(threads[i], dc);
    }
    pthread_mutex_destroy(&lock);

    memcpy(arr, out, sizeof(unsigned int) * size);
    free(out);
    free(threads);
}

int main(int argc, char * argv[])
{
    int i;
    int size = atoi(argv[1]);
    unsigned int * arr = malloc(sizeof(unsigned int) * size);

    /* Generate random array by reading /dev/urandom. It's totally better
     * than rand() */
    int fp = open("/dev/urandom", O_RDONLY);
    for (i = 0; i < size; ++i)
    {
        read(fp, &arr[i], sizeof(unsigned int));
        arr[i] %= 100;
    }
    close(fp);

    sleep_sort(arr, size);

    /* Pretty print it */
    printf("[");
    for (i = 0; i < size - 1; ++i)
    {
        printf("%d, ", arr[i]);
    }
    printf("%d]\n", arr[size - 1]);

    free(arr);
    return 0;
}


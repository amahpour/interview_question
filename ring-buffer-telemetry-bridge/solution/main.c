#include "ringbuf.h"
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define CAPACITY 64
static unsigned char storage[CAPACITY];

void *producer(void *_) {
    unsigned char x = 0;
    while (1) {
        rb_push(x++);
        usleep(1000);
    }
}

int main(void) {
    rb_init(storage, CAPACITY);

    pthread_t th;
    pthread_create(&th, NULL, producer, NULL);

    unsigned char v;
    while (1) {
        if (rb_pop(&v)) {
            printf("%u\n", v);
            fflush(stdout);
        }
    }
}

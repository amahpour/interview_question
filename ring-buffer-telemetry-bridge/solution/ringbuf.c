#include "ringbuf.h"
#include <pthread.h>

static unsigned char *store     = NULL;
static size_t          cap      = 0;
static size_t          head     = 0;
static size_t          tail     = 0;
static pthread_mutex_t lock     = PTHREAD_MUTEX_INITIALIZER;

static inline size_t next(size_t idx) { return (idx + 1U) % cap; }

void rb_init(unsigned char *buf, size_t len)
{
    pthread_mutex_lock(&lock);
    store = buf;
    cap   = len;
    head  = tail = 0;
    pthread_mutex_unlock(&lock);
}

bool rb_push(unsigned char byte)
{
    bool ok = false;
    pthread_mutex_lock(&lock);

    size_t nxt = next(head);
    if (nxt != tail) {
        store[head] = byte;
        head = nxt;
        ok = true;
    }

    pthread_mutex_unlock(&lock);
    return ok;
}

bool rb_pop(unsigned char *out)
{
    bool ok = false;
    pthread_mutex_lock(&lock);

    if (head != tail) {
        *out = store[tail];
        tail = next(tail);
        ok = true;
    }

    pthread_mutex_unlock(&lock);
    return ok;
}

#ifndef RINGBUF_H
#define RINGBUF_H
#include <stdbool.h>
#include <stddef.h>

void  rb_init(unsigned char *buffer, size_t capacity);
bool  rb_push(unsigned char byte);
bool  rb_pop (unsigned char *out_byte);

#endif

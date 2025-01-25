#include "trace.h"

#define BUF_LEN 128

static char iringbuf[BUF_LEN];
static char *iringbuf_head = NULL;
static char *iringbuf_tail = iringbuf_head + BUF_LEN -1 ;


void itrace() {
    iringbuf_head = malloc();

   
    free();

}



    cb->buffer = malloc(capacity * sz);
    cb->buffer_end = (char *)cb->buffer + capacity * sz;
    cb->capacity = capacity;
    cb->count = 0;
    cb->sz = sz;
    cb->head = cb->buffer;
    cb->tail = cb->buffer;
    free(cb->buffer);


void cb_push_back(circular_buffer *cb, const void *item)
{
    if(cb->count == cb->capacity){
        // handle error
    }
    memcpy(cb->head, item, cb->sz);
    cb->head = (char*)cb->head + cb->sz;
    if(cb->head == cb->buffer_end)
        cb->head = cb->buffer;
    cb->count++;
}

void cb_pop_front(circular_buffer *cb, void *item)
{
    if(cb->count == 0){
        // handle error
    }
    memcpy(item, cb->tail, cb->sz);
    cb->tail = (char*)cb->tail + cb->sz;
    if(cb->tail == cb->buffer_end)
        cb->tail = cb->buffer;
    cb->count--;
}

void mtrace() {


}

void ftrace() {

}

void dtrace() {
    
}
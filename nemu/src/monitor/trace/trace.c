#include "trace.h"

#define BUF_SIZE 20
#define BUF_LEN 128 

static char *iringbuf[BUF_LEN];
static char *iringbuf_head = NULL;
static char *iringbuf_tail = NULL ;
static char *iringbuf_curr = NULL;


void iringbuf(char *logbuf) {
    iringbuf_head = malloc(BUF_SIZE * BUF_LEN);

    if(iringbuf_head == NULL) assert(0);

    iringbuf_tail = iringbuf_head + BUF_SIZE * BUF_LEN - 1;

    iringbuf_curr = iringbuf_head;

    strcpy(iringbuf_head,logbuf);

    iringbuf_curr += iringbuf_head;

    if(iringbuf_curr >= iringbuf_tail) {
        iringbuf_curr = iringbuf_head;
    }
    free(iringbuf_head);
}

void itrace_display(char *logbuf) { 
    if()  {
        Log("-->%s",logbuf);
    } else {
        Log("%s",logbuf);
    } 
}



void mtrace() {


}

void ftrace() {

}

void dtrace() {

}
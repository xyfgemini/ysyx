#ifndef __TRACE_H__
#define __TRACE_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <debug.h>
#include <memory/paddr.h>

void iringbuf_w(char*);

void itrace_display();

void ftrace_init(const char*);

void ftrace_display(bool,bool,word_t,word_t);

void ftrace_free();


#endif
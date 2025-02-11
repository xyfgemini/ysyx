#ifndef __TRACE_H__
#define __TRACE_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <debug.h>

void iringbuf_w(char*);

void itrace_display();

static bool ftrace_is_Elf_32(FILE *); 

static char *ftrace_get_func_name(word_t);

void ftrace_init(const char*);

void ftrace_display(bool,bool,word_t,word_t);

void ftrace_free();


#endif
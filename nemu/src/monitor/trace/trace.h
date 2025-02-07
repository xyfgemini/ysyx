#ifndef __TRACE_H__
#define __TRACE_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <debug.h>

void iringbuf_w(char *);

void itrace_display();

void ftrace_init(char *);

#endif
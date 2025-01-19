#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
    return 0;

}

int vsprintf(char *out, const char *fmt, va_list ap) {
    return 0;

}

int sprintf(char *out, const char *fmt, ...) {
    // va_list args;
    // char ch;
    // int count = 0,length = 0;
    // va_start(args,fmt);
    // while(ch = *fmt++) {
    //     //常规
    //     *out++ = ch;
    //     ++count;
    //     //%s %d
    //     if(ch == '%') {
    //         *out++ = ch;
    //         ++count;
    //         switch(ch = *fmt++) {
    //             case 's':  *out +=  va_arg(args,char *);
    //             case 'd':  *out++ = va_arg(args,int);
    //                         count += length;
    //                         break;
    //             default:  break;
    //         }
    //     }  
    // }
    // *out = '\0';
    // va_end(args);
    // return count;
    return 0;
}

//
int snprintf(char *out, size_t n, const char *fmt, ...) {
    return 0;
  

}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
    return 0;

}

#endif

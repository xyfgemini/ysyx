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
    va_list args;
    char ch;
    char *buf;
    char buf_tmp[512];
    char *string_tmp;
    int int_tmp;
    int count = 0;
    int length = 0;

    va_start(args,fmt);

    while(ch = *fmt++) {
        *buf++ = ch;
        ++count;
        //%s %d
        if(ch == '%') {
            *buf++ = ch;
            ++count;
            switch(ch = *fmt++) {
                case 's':  
                    string_tmp = va_arg(args,char*); 
                    length = strlen(string_tmp); 
                    buf = string_tmp;
                    buf += length; 
                    count += length;
                    break;

                case 'd':  
                    int_tmp = va_arg(args,int); 
                    itoa(int_tmp,buf_tmp,10);
                    length = strlen(buf_tmp);
                    buf = buf_tmp;
                    buf += length;
                    count += length;
                    break;

                default:  
                    break;
            }
        }  
    }
    *buf = '\0';
    va_end(args);
    out = buf;
    return count;
}


int snprintf(char *out, size_t n, const char *fmt, ...) {
    return 0;
  

}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
    return 0;

}

#endif

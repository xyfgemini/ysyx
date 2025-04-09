#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

char *out_tmp = NULL;

char num_arr[256];
char *num_arr_ptr = NULL;

void PrintDec(int d) {
    if(d == 0) {
        return;
    } else {
        PrintDec(d/10);
        //trick:以字符串开头的数组形式的表达式，它会根据数组下表，找到对应字符作为输出
        char ch = "0123456789"[d % 10];
        *num_arr_ptr = ch;
        num_arr_ptr++;
    }
}

void dec_display(int d) {
    if(d == 0) {
       *out_tmp = '\0';
       out_tmp ++; 
    } else if(d < 0) {
        *out_tmp = '-';
        out_tmp ++;
        d = -d;
    } else {
        memset(num_arr,'\0',256);
        num_arr_ptr = num_arr;
        PrintDec(d);
        memcpy(out_tmp,num_arr,strlen(num_arr));
        out_tmp += strlen(num_arr);
    }
}

void char_display(char *s) {
    memcpy(out_tmp,s,strlen(s));
    out_tmp += strlen(s);
}

int sprintf(char *out, const char *fmt, ...) {
    va_list args;
    va_start(args,fmt);
    char ch;
    memset(out,'\0',128);
    out_tmp = out;
    while((ch = *fmt) != '\0') {
        if(ch == '%') {
            fmt++;
            switch(*fmt) {
                case 's': char_display(va_arg(args,char*)); break;
                case 'd': dec_display(va_arg(args,int)); break;
                default: break;
            }
        } else {
            *out_tmp = ch;
            out_tmp ++;
        }
        fmt++;
    }
    out_tmp = '\0';
    va_end(args);
    return 0;
}

/***********************************************************************/

void PrintNum(unsigned int d,int base) {
     if(d == 0) {
        return;
    } else {
        PrintNum(d/base,base);
        char ch = "0123456789abcdef"[d % base];
        putch(ch);
    }    
}


int printf(const char *fmt, ...) {
    va_list args;
    va_start(args,fmt);
    while(*fmt != '\0') {
        if(*fmt == '%') {
            fmt++;
            switch(*fmt) {
                case '%': putch(*fmt); break;
                case 's': putstr(va_arg(args,char*)); break;
                case 'd': PrintNum(va_arg(args,int),10); break;
                case 'x': PrintNum(va_arg(args,unsigned int),16); break;
            }
        } else {
            putch(*fmt);
        }
        fmt++;
    }
    va_end(args);
    return 0;
}



int vsprintf(char *out, const char *fmt, va_list ap) {
    return 0;

}

int snprintf(char *out, size_t n, const char *fmt, ...) {
    return 0;
  

}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
    return 0;

}

#endif

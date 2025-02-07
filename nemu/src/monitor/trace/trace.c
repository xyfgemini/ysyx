#include "trace.h"

#define BUF_LEN 10

static char *iringbuf[BUF_LEN];
static char **iringbuf_end = iringbuf + BUF_LEN - 1;
static char **iringbuf_head = NULL;
static char **iringbuf_curr = NULL;


// 执行一条指令写入缓冲区
void iringbuf_w(char *logbuf) {
    if(iringbuf_head == NULL) {
        iringbuf_head = iringbuf;
    } 
    if(iringbuf_head == iringbuf_end) {
        iringbuf_head = iringbuf;
    } else {
        iringbuf_head++;
    }
    if(*iringbuf_head == NULL) {
        *iringbuf_head = (char *)malloc(sizeof(char) * 512);
    }
    strcpy(*iringbuf_head,logbuf);
    iringbuf_curr = iringbuf_head;
}

//程序出错，打印出缓冲区中的指令
void itrace_display() { 
    iringbuf_head = iringbuf;
    while(iringbuf_head != NULL && iringbuf_head != iringbuf_end) {
        if(iringbuf_head == iringbuf_curr) {
            printf("--->%s\n",*iringbuf_head);
        } else {
            printf("    %s\n",*iringbuf_head);
        }
        iringbuf_head++;
    }
}

bool is_Elf_type_32(FILE *fp) {




}

void ftrace(const char *elf) {
    FILE *fp = fopen("elf","r");
    Assert(fp != NULL,"can not open %s",elf);
    Assert(is_Elf_type_32(fp),"%s is not 32 bit version",elf);

    Elf32_Ehdr elf_header;
    //读取elf header

    //读取elf section header

    //读取elf .symtab


    //读取elf .strtab


    //riscv32 jal jalr辨别call和return
    


    fclose(fp);
}

void ftrace_display() {



}

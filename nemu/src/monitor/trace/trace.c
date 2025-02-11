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

//存储ELF符号表中函数名
#define ARR_LEN (1024 * 1024)
static char *func_name_arr[ARR_LEN];

//存储call ret函数名称
static int depth = 0;
static char *inst_func_name_arr[ARR_LEN];
static char **inst_func_name_head = inst_func_name_arr;

//存储调试信息
static char *ftrace_info_arr[ARR_LEN];
static char **ftrace_info_head = ftrace_info_arr;


static bool ftrace_is_Elf_32(FILE *fp) {
    char buf[16];
    //nmemb--->16
    int num = fread(buf,1,16,fp);
    //读到的number小于16,则不是ELF文件格式；
    if(num < 16) {
        return false;
    }
    //不是ELF32格式文件
    if(buf[EI_CLASS] != ELFCLASS32) {
        return false;
    }
    return true;
}


static char *ftrace_get_func_name(word_t addr) {
    word_t offset = addr - CONFIG_MBASE; //0x80000000
    if(func_name_arr[offset] != NULL) {
        return func_name_arr[offset];
    } else {
        return (char*)"\0";
    } 
}


void ftrace_init(const char *elf) {
    FILE *fp = fopen("elf","r");
    Assert(fp != NULL,"can not open '%s'",fp);
    Assert(ftrace_is_Elf_32(fp),"'%s' is not 32bit type",fp);

    //read elf header
    Elf32_Ehdr elf_header;
    fseek(fp,0,SEEK_SET);
    fread(&elf_header,1,elf_header.e_ehsize,fp);

    //read elf section header
    //section descripter ---> struct array
    //sizeof(Elf32_Shdr)==elf_header.e_shentsize
    Elf32_Shdr elf_section_header[elf_header.e_shnum];
    fseek(fp,elf_header.e_shoff,SEEK_SET);
    fread(elf_section_header,elf_header.e_shentsize,elf_header.e_shnum,fp);

    //read elf_section_header-->.symtab .strtab
    Elf32_Shdr elf_section_sym;
    Elf32_Shdr elf_section_str;
    for(int i = 0;i < elf_header.e_shnum;i++) {
        if(elf_section_header[i].sh_type == SHT_SYMTAB) {
            memcpy(&elf_section_sym,&elf_section_header[i],sizeof(Elf32_Shdr));
            if(elf_section_header[elf_section_sym.sh_link].sh_type == SHT_STRTAB) {
                memcpy(&elf_section_str,&elf_section_header[elf_section_sym.sh_link],sizeof(Elf32_Shdr));
            }
        }
    }

    //read .symtab 
    Elf32_Sym elf_sym_arr[elf_section_sym.sh_size / elf_section_sym.sh_entsize];
    fseek(fp,elf_section_sym.sh_offset,SEEK_SET);
    fread(&elf_sym_arr,1,elf_section_sym.sh_size,fp);

    //read .strtab
    char elf_str_arr[elf_section_str.sh_size];
    fseek(fp,elf_section_str.sh_offset,SEEK_SET);
    fread(&elf_str_arr,1,elf_section_str.sh_size,fp);

    for(int j = 0;j < elf_section_sym.sh_size / elf_section_sym.sh_entsize;j++) {
        if(elf_sym_arr[j].st_info == STT_FUNC) {
            Elf32_Addr value = elf_sym_arr[j].st_value; 
            Elf32_Word name = elf_sym_arr[j].st_name; //name属性-->字符串在字符串表中的偏移量
            Elf32_Word offset = value - elf_header.e_entry;
            char *func_name = elf_str_arr + name;
            if(func_name_arr[offset] == NULL) {
                func_name_arr[offset] = (char *)malloc(sizeof(char) * 512);
            }
            strcpy(func_name_arr[offset],func_name);
        }
    }

    fclose(fp);
}

static char buf_total[1024];
static char buf_former[512];
static char buf_latter[512];

void ftrace_display(inst_call,inst_ret,pc,dnpc) {
    if(ftrace_get_func_name(dnpc) == "\0") {
        return ;
    }

    if(inst_call || inst_ret) { 
        sprintf(buf_former,"[ftrace]addr:0x%08" PRIX32,(word_t)pc);
    }

    if(inst_call) {
        if(*inst_func_name_head == NULL) {
            *inst_func_name_head = (char *)malloc(512);
        }
        strcpy(*inst_func_name_head,ftrace_get_func_name);
        sprintf(buf_latter,"call [%s@0x%08" PRIX32 "]\n",*inst_func_name_head,(word_t)dnpc);
        inst_func_name_head++;
        depth++;
    }

    if(inst_ret) {
        if() {
            
        } else {
            inst_func_name_head--;
            depth--;
        }
        sprintf(buf_latter," ret[%s]\n",*inst_func_name_head);
    }

    if(inst_call || inst_ret) {
        sprintf(buf_total,"%s %s",buf_former,buf_latter);
    }
}


void ftrace_free() {
    for(int i=0;i < ARR_LEN;i++) {
        if(func_name_arr[i] != NULL) {
            free(func_name_arr[i]);
        }
        if(inst_func_name_arr[i] != NULL) {
            free(inst_func_name_arr[i]);
        }
        if(ftrace_info_arr[i] != NULL) {
            free(ftrace_info_arr[i]);
        }
    }
}


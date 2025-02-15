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

// void mtrace_display(word_t addr,word_t data) {
//     addr = CONFIG_MBASE + i * 4;
//     data = paddr_read(addr,4);
//     Log("[mtrace] addr:" FMT_WORD "data :" FMT_WORD "\n",addr,data);
// }

//存储ELF符号表中函数名
#define ARR_LEN (1024 * 1024 * 10)
static char *func_name_arr[ARR_LEN];

//存储call ret函数名称
static int depth = 0;
static char *inst_func_name_arr[ARR_LEN];
static char **inst_func_name_head = inst_func_name_arr;

//存储调试信息
static char *ftrace_info_arr[ARR_LEN];
static char **ftrace_info_head = ftrace_info_arr;

static bool ftrace_is_Elf_32(FILE *); 
static char* ftrace_get_func_name(word_t);

static bool ftrace_is_Elf_32(FILE *fp) {
    char buf[16];
    //nmemb--->16
    int num = fread(buf,1,16,fp);
    //读到的number小于16,则不是ELF文件格式；
    if(num < 16) {
        return false;
    }
    //不是ELF32格式文件
    if(buf[0] != 0x7f || buf[1] != 'E' || buf[2] != 'L' || buf[3] != 'F' || buf[EI_CLASS] != ELFCLASS32) {
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


void ftrace_init(const char *elf_file) {
    FILE *fp = fopen(elf_file,"rb");
    Assert(fp,"Can not open '%s'",elf_file);
    Assert(ftrace_is_Elf_32(fp),"'%s' is not Elf32 format",elf_file);

    //read elf header
    Elf32_Ehdr elf_header;
    // printf("%d %ld\n",elf_header.e_ehsize,sizeof(elf_header));
    fseek(fp,0,SEEK_SET);
    int ret0 = fread(&elf_header,sizeof(elf_header),1,fp);
    assert(ret0 == 1);

    //read elf section header
    //section descripter ---> struct array
    Elf32_Shdr elf_section_header[elf_header.e_shnum];
    fseek(fp,elf_header.e_shoff,SEEK_SET);
    //printf("%d %d %ld\n",elf_header.e_shentsize,sizeof(Elf32_Shdr),elf_header.e_shnum);
    //sizeof(Elf32_Shdr)==elf_header.e_shentsize
    int ret1 = fread(elf_section_header,elf_header.e_shentsize,elf_header.e_shnum,fp);
    // printf("%d %d\n",ret1,elf_header.e_shnum);
    assert(ret1 == elf_header.e_shnum);

    //read elf_section_header-->.symtab .strtab
    Elf32_Shdr elf_section_sym;
    Elf32_Shdr elf_section_str;
    //printf("%d %d\n",elf_section_sym.sh_size,elf_section_sym.sh_entsize);
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
    int ret2 = fread(elf_sym_arr,elf_section_sym.sh_size,1,fp);
    assert(ret2 == 1);

    //read .strtab
    char elf_str_arr[elf_section_str.sh_size];
    fseek(fp,elf_section_str.sh_offset,SEEK_SET);
    //printf("%d %ld\n",elf_section_str.sh_size,sizeof(elf_str_arr));
    int ret3 = fread(elf_str_arr,elf_section_str.sh_size,1,fp);
    assert(ret3 == 1);

    for(int j = 0;j < elf_section_sym.sh_size / elf_section_sym.sh_entsize;j++) {
        // printf("%d %d\n",elf_sym_arr[j].st_info,STT_FUNC);
        if(ELF32_ST_TYPE(elf_sym_arr[j].st_info) == STT_FUNC) {
            Elf32_Addr value = elf_sym_arr[j].st_value; 
            Elf32_Word name = elf_sym_arr[j].st_name; //name属性-->字符串在字符串表中的偏移量
            Elf32_Word offset = value - elf_header.e_entry;
            char *func_name = elf_str_arr + name;
            if(func_name_arr[offset] == NULL) {
                func_name_arr[offset] = (char *)malloc(sizeof(char) * 512);
            }
            strcpy(func_name_arr[offset],func_name);
#ifdef CONFIG_FTRACE_ELF
        Log("[ftrace] symbol address:" FMT_WORD "\n" ,value);
        Log("[ftrace] function name: %s\n",func_name);
#endif
        }
    }
    fclose(fp);
}

static char buf_total[1024];
static char buf_former[512];
static char buf_latter[512];

void ftrace_display(bool inst_call,bool inst_ret,word_t pc,word_t dnpc) {
    //递归调用
    if(inst_call && strcmp(ftrace_get_func_name(dnpc),"\0")) {
        return ;
    }

    if(inst_call || inst_ret) { 
        sprintf(buf_former,"[ftrace]addr:0x%08" PRIX32,(word_t)pc);
    }

    if(inst_call) {
        if(*inst_func_name_head == NULL) {
            *inst_func_name_head = (char *)malloc(512);
        }
        strcpy(*inst_func_name_head,ftrace_get_func_name(dnpc));
        sprintf(buf_latter,"call [%s@0x%08" PRIX32 "]\n",*inst_func_name_head,(word_t)dnpc);
        inst_func_name_head++;
        depth++;
    }

    if(inst_ret) {
        if(inst_func_name_head == inst_func_name_arr) { 
            return;
        } else {
            inst_func_name_head--;
            depth--;
        }
        sprintf(buf_latter," ret[%s]\n",*inst_func_name_head);
    }

    if(inst_call || inst_ret) {
        sprintf(buf_total,"%s %s",buf_former,buf_latter);
        if(*ftrace_info_head == NULL) {
            *ftrace_info_head = (char *)malloc(1024);
        }
        strcpy(*ftrace_info_head,buf_total);
        ftrace_info_head++;
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


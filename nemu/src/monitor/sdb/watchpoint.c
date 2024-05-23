/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include "sdb.h"
#include <regex.h>


#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  char expr[32];
  word_t value;
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
static int number = 0; //watchpoint数量
char addr[100];

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

void new_wp(char *args){
  bool success = false;
  if(free_ == NULL){
    assert(0);
  }
  WP* buf = free_;
  free_ = free_->next;//提前变化
  if(head == NULL){
    head = buf;
    head->next = NULL;
    // printf("%p\n%p\n%p\n",head,free_,free_->next);
  }
  else{
    buf->next = head;
    head = buf;
    // printf("%p\n%p\n",head,buf->next);
  }
  head->NO = ++number;
  strcpy(head->expr,args); //数组or指针不能直接给数组赋值
  head->value = expr(args,&success);
 
  if(success){
    printf("Hardware Watchpoint %d:%s\nvalue = %u\n",head->NO,head->expr,head->value);
  }
  else{
    printf("can't set watchpoint!");
  }
}


void free_wp(char* args){
  WP* prev = head;
  int num = 0;
  sscanf(args,"%d",&num);
  if(head->NO == num){
    head = head->next;
    prev->next = free_;
    free_ = prev;
  }
  while(prev->next != NULL){
    if(prev->next->NO == num){
      prev->next = prev->next->next;
      prev->next->next = free_;
      free_ = prev->next;
      return ; //learning
    }
    prev = prev -> next;
  }
  printf("watchpoint has not been freed!\n");
}


void display_watchpoint(){
    WP* wp = head; //需要缓冲指针,不能动head;
    if(wp == NULL){
        printf("no watchpoints have been set!\n");
        return ;
    }
    printf("num\texpr\tvalue\n");
    while(wp != NULL){
        printf("%d\t%s\t%u\n",wp->NO,wp->expr,wp->value);
        // printf("%p %p",head,wp);
        wp = wp->next;
    }
}

//设置断点 w $pc==0x80000010
bool new_bp(char *args){
    regex_t preg;
    const char *pattern = "^\\$pc==0x[0-9a-fA-F]+$";
    int cflags = REG_EXTENDED;
    size_t nmatch = 1;
    regmatch_t pmatch[1];
    int status;
    regcomp(&preg,pattern,cflags);
  
    status = regexec(&preg,args,nmatch,pmatch,0);
    if(status == REG_NOMATCH){
      return false;
    }
    else if(status == 0){
      strcpy(addr,args+5);
      return true;
    }
    regfree(&preg);
    return false;
}

//扫描监视点
bool inspect_watchpoint(){
    WP *cur = head;
    bool success = false;
    static int index = 0;
    uint32_t hex_addr = 0;
    sscanf(addr,"%x",&hex_addr);
    if(cpu.pc == hex_addr){
      printf("breakpoint %d at 0x%x\n",++index,cpu.pc);
      return true;
    }
    while(cur!=NULL){
      if(cur->value != expr(cur->expr,&success)){
        printf("watchpoint %d:%s\nOld value=%u\nNew value=%u\n",cur->NO,cur->expr,cur->value,expr(cur->expr,&success));
        return true;
      }
      cur = cur->next;
    }
    return false;
}










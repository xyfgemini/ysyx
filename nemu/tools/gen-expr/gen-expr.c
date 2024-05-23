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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

uint32_t choose(uint32_t n){
  uint32_t a = 0;
  srand(unsigned(time(NULL)));
  a = rand() % n;
  return a;
}

void gen_num(){
  uint32_t a = 0;
  srand(unsigned(time(NULL)));
  a = rand() % 100 + 1;
  strcat(buf,a);
}

void gen_rand_op(){
  int index = 0;
  char a[]={'+','-','*','/'};
  srand(unsigned(time(NULL)));
  index = choose(4);
  strcat(buf,a[index]);
}

void gen(int parentheses){
  char pt[]={'(',')'};
  switch(parentheses){
    case '(':strcat(buf,'(');
    case ')':strcat(buf,')');
    default:assert(0);break;
  }
}

void gen_rand_blank(){
  srand(time(0));
  if (rand() % 2 == 0){
    strcat(buf,' ');
  }
}

//设置递归深度,buf不会溢出
static void gen_rand_expr(int depth) {
    switch (choose(n)) {
      case 0: gen_rand_blank();gen_num(); gen_rand_blank();depth--;break;
      case 1: gen_rand_blank();gen('('); gen_rand_expr(); gen_rand_blank();gen(')'); gen_rand_blank();depth--;break;
      default: gen_rand_blank();gen_rand_expr(); gen_rand_op(); gen_rand_expr(); gen_rand_blank();depth--;break;
    }
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    buf[0] = '\0';//静态变量
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);//buf-->code_buf

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);//code_buf-->fp
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -Wall -Werror -o /tmp/.expr");//return 0 if success
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result); 
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}

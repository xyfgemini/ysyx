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

#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <memory/paddr.h>

enum {
  TK_NOTYPE=256,TK_EQ=257,TK_NEQ=258,
  TK_REG=259,TK_HEX=260,TK_NUM=261,TK_AND=262,TK_OR=263,
  TK_DEREF,TK_MINUS
};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {
  //precedence
  {"0x[0-9a-fA-F]+",TK_HEX},
  {"[0-9]+",TK_NUM},
  {"\\$[$a-z0-9]+",TK_REG},
  {" +", TK_NOTYPE},    // spaces
  {"\\+",'+'}, 
  {"\\-",'-'},
  {"\\*",'*'},
  {"\\/",'/'},
  {"\\(",'('},
  {"\\)",')'},    
  {"==", TK_EQ},        
  {"!=",TK_NEQ},        
  {"&&",TK_AND},
  {"\\|\\|",TK_OR}, 
};

#define NR_REGEX ARRLEN(rules)

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len; 

        if(substr_len>32) {assert(0);}  //处理输入过长的表达式

        switch (rules[i].token_type) {
          case TK_NOTYPE:  break; 
          case '-': 
          case '*': if(tokens[nr_token-1].type==')' || tokens[nr_token-1].type==TK_HEX || \
                    tokens[nr_token-1].type==TK_REG ||tokens[nr_token-1].type==TK_NUM)
                      tokens[nr_token].type = rules[i].token_type;
                    else if(nr_token==0 || tokens[nr_token-1].type=='(' || tokens[nr_token-1].type=='+'||\
                    tokens[nr_token-1].type=='-' || tokens[nr_token-1].type=='*' || tokens[nr_token-1].type=='/'||\
                    tokens[nr_token-1].type==TK_EQ || tokens[nr_token-1].type==TK_NEQ || \
                    tokens[nr_token-1].type==TK_AND || tokens[nr_token-1].type==TK_OR){
                      switch(rules[i].token_type) {
                        case '-': tokens[nr_token].type = TK_MINUS;break;
                        case '*': tokens[nr_token].type = TK_DEREF;break;
                      }
                    }
                    else {
                        assert(0);
                    }
                    nr_token++;
                    break;
          
          case TK_REG: 
          case TK_HEX: 
          case TK_NUM:  strncpy(tokens[nr_token].str,substr_start,substr_len);
                        tokens[nr_token].str[substr_len]='\0';
                    
          default:      tokens[nr_token].type=rules[i].token_type;
                        // printf("%s %d\n",tokens[nr_token].str,tokens[nr_token].type);
                        nr_token++;
                        break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  
  return true;
}


//检查表达式是否被一对匹配的括号包围着:stack
/*创建stack，(进栈，)判断栈是否为空，栈空false,非空，判断是否匹配
*最后若栈空，匹配成功
*/
bool check_parentheses(int p,int q){
  int index = p+1,flag = 0;
  char stack[1024] ="\0";
  if(tokens[p].type=='(' && tokens[q].type==')'){
    while(index<q){
      if(tokens[index].type=='('){
        stack[flag]='(';
        flag++;
      }
      if(tokens[index].type==')'){
        if(stack[flag]!='(')  return false;//()+()首尾部括号不匹配
        if(flag<0) assert(0);  //()) 不合法 *positon = -1
        flag--;
      }
      index++;
    }
    if(flag==0) return true;
  }
  return false;
} 

//主运算符不在括号内
bool annihilation(int p,int q){
  int index = p+1;
  int flag = 0;
  for(;index<=q;index++){
    if(tokens[index].type=='(') flag++;
    if(tokens[index].type==')') flag--;
  }
  if(flag==0) return true;
  return false;
}


//find the position of "主运算符"(表达式合法)
int main_operator(int p,int q) {
  int index = p;
  int flag1=0,flag2=0,flag3=0,flag4=0,flag5=0;
  int num1=0,num2=0,num3=0,num4=0,num5=0;
  while(index <= q){
    //1.非运算符
    if(tokens[index].type!= TK_HEX && tokens[index].type != TK_NUM \
        && tokens[index].type != TK_REG && tokens[index].type != '('\
        && tokens[index].type != ')') {
          //2.()内的不是主运算符(不会出现括号包含整个表达式)
          if(annihilation(index,q)){
          //3.主运算符的优先级最低(结合性)
            switch(tokens[index].type) {
              case '*':    
              case '/':      flag1=index;num1++;break;
              case '+':       
              case '-':      flag2=index;num2++;break;
              case TK_EQ:
              case TK_NEQ:   flag3=index;num3++;break;
              case TK_AND:   flag4=index;num4++;break;
              case TK_OR:    flag5=index;num5++;break;
              default:       assert(0);break;
            }
          }
    }
    index++;
  } 
  // printf("%d %d %d %d %d\n",num5,num4,num3,num2,num1);
  //find the main operator
  if(num5!=0){
    return flag5;
  }
  else if(num4!=0){
    return flag4;
  }
  else if(num3!=0){
    return flag3;
  }
  else if(num2!=0){
    return flag2;
  }
  else if(num1!=0){
    return flag1;
  }
  else{
    printf("Unsupported operator!\n");
  }
  return 0;
}

//recursion
word_t eval(int p,int q){
  // printf("eval:%d %d %d\n",p,q,check_parentheses(p,q));
  int op  = 0;
  word_t val = 0,val1 = 0,val2 = 0;
  if(p > q){
    printf("bad expressions!\n");
    assert(0);
  }
  else if(p==q){
    if(tokens[p].type==TK_NUM){
      sscanf(tokens[p].str,"%u",&val);
      return val;
    }
    else if(tokens[p].type==TK_HEX){
      sscanf(tokens[p].str,"0x%x",&val);//将字符串按%x的方式进行解析?
      return val;
    }
    else if(tokens[p].type==TK_REG){
      bool* success = false;
      val = isa_reg_str2val(tokens[p].str,success);
      return val;
    }
    else { 
      assert(0);
    }
  }
  else if(tokens[p].type==TK_DEREF || tokens[p].type==TK_MINUS){
    //-(5+3) (-5+3) *($a0+5) (*0x80000000)+5 -(-5)
    if(check_parentheses(p+1,q) || q-p==1) {
      switch(tokens[p].type){
        case TK_DEREF:return -eval(p+1,q);
        case TK_MINUS:return *((uint32_t*)guest_to_host(eval(p+1,q)));
        default:assert(0);break;
      }
    }
  }
  else if(check_parentheses(p,q)){
    return eval(p+1,q-1);
  }
  //表达式一定合法
  else{
    op = main_operator(p,q);
    val1 = eval(p,op-1);
    val2 = eval(op+1,q);
    switch(tokens[op].type){
      case '+':return val1 + val2;
      case '-':return val1 - val2;
      case '*':return val1 * val2;
      case '/':return val1 / val2;
      case TK_AND:return val1 && val2;
      case TK_OR:return val1 || val2;
      case TK_EQ:return val1 == val2;
      case TK_NEQ:return val1 != val2;
      default:assert(0);
    }
  }
  return 0;
}

//负数 指针解引用
word_t expr(char *e, bool *success) {
    word_t result = 0;
    if (!make_token(e)) {
      *success = false;
      return 0;
    }
    int p = 0 , q = nr_token - 1;
    *success = true;
    result = eval(p,q);
    return result;
}

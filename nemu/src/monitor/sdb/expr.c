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
#include<memory/vaddr.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
/*目前只有十进制四则运算*/
enum {
  TK_NOTYPE = 256,//空格
  TK_EQ=1,//==
  TK_NUM=2,//十进制整数
  TK_NEQ=3,//!=
  TK_AND=4,//&&
  TK_HEX=5,//16进制
  TK_REG=6,//寄存器$开头
  TK_DEREF=7,//解指针

  /* TODO: Add more token types */

};

static struct rule {
  const char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},		//sub
  {"\\*", '*'},		//mul或指针
  {"\\/", '/'},		//div
  {"\\(", '('},		//左括弧
  {"\\)", ')'},		//右括弧
  {"0[xX][0-9a-fA-F]+",TK_HEX},		//16进制
  {"[0-9]+",TK_NUM},
  {"\\=\\=", TK_EQ},    // equal
  {"\\!\\=",TK_NEQ},	//不等
  {"\\&\\&",TK_AND},    //&&
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

static Token tokens[65535] __attribute__((used)) = {};
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

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        Token token_getted;//貌似不要也罢；
        switch (rules[i].token_type) {
		case'+':{	//printf("%d\n",nr_token);
				token_getted.type='+';
				tokens[nr_token++]=token_getted;
				//printf("%d\n",nr_token);
				break;
			}
		case'-':{
			        token_getted.type='-';
				tokens[nr_token++]=token_getted;
				break;
			}
		case'*':{
				token_getted.type='*';
				tokens[nr_token++]=token_getted;
				break;
			}
		case'/':{
				token_getted.type='/';
				tokens[nr_token++]=token_getted;
				break;
			}
		case TK_NOTYPE:{
				break;
			       }
		case TK_NUM:{//基于假设，num其实是个unsigned int
			       tokens[nr_token].type=TK_NUM;
			       if(substr_len>=32)
			       {
				       assert(0);
			       }
			       strncpy(tokens[nr_token].str,substr_start,substr_len);
			       tokens[nr_token].str[substr_len]='\0';
			       nr_token++;
			       break;
			    }
		case '(':{
				   token_getted.type='(';
				   tokens[nr_token++]=token_getted;
				   break;
			   }
		case ')':{
				   token_getted.type=')';
				   tokens[nr_token++]=token_getted;
				   break;
			   }
		case TK_NEQ:{
				   tokens[nr_token].type=TK_NEQ;
				   strcpy(tokens[nr_token++].str,"!=");
		 	   
				   break;
			    }
		case TK_AND:{
			 tokens[nr_token].type=TK_AND;
			 strcpy(tokens[nr_token++].str,"&&");
		 
			 break;
		 }		 
		case TK_EQ:{
				   tokens[nr_token].type=TK_EQ;
				   strcpy(tokens[nr_token++].str,"==");
			   }
		case TK_REG:{
				    tokens[nr_token].type=TK_REG;
				    if(substr_len>=32)
				    {
					    assert(0);
				    }
				    strncpy(tokens[nr_token].str,substr_start,substr_len);
				    tokens[nr_token++].str[substr_len]='\0';
				    break;
			    }
		case TK_HEX:{
				    tokens[nr_token].type=TK_HEX;
				    if(substr_len>=32)
				    {
					    assert(0);
				    }
				    strncpy(tokens[nr_token].str,substr_start,substr_len);
				    tokens[nr_token++].str[substr_len]='\0';
				    break;
			    }
		default:break; //TODO();//错误输入提醒还没写
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
bool check_parentheses(int p,int q){
	if(tokens[p].type!='('||tokens[q].type!=')')
		return false;
	else{
		int i=p+1;
		int num=0;
		for(;i<q;i++)
		{
			if(tokens[i].type=='(')
				num++;
			else if(tokens[i].type==')')
				num--;

			if(num<0) return false;
		}
		if(num!=0)
			return false;

	}
	return true;
}
uint32_t evalnum(int i,bool *ok){
	switch(tokens[i].type)
	{
		case TK_HEX:
			return strtol(tokens[i].str,NULL,16);
		case TK_NUM:
		       	return strtol(tokens[i].str,NULL,10);
		case TK_REG:
			return isa_reg_str2val(tokens[i].str,ok);
			
		default:*ok=false;return 0;
	}
	}

uint32_t solve2(uint32_t val1,int op_type,uint32_t val2,bool *ok)
{
	
		switch(op_type){
			case'+':return val1+val2;
			case'-':return val1-val2;
			case'*':return val1*val2;
			case'/':
				{if(val2==0){

					*ok=false;
					return 0;
					    }
					return val1/val2;
				}
			case TK_EQ:return val1==val2;
			case TK_AND:return val1&&val2;
			case TK_NEQ:return val1!=val2;
			default:
				*ok=false;
				return 0;
				
		}
}

uint32_t solve1(uint32_t op_type,uint32_t val2,bool *ok)//解指针
{
	if(op_type!=TK_DEREF)
	{
		*ok=false;
		return 0;
	}
	else
	{//	printf("0x%xval2\n",val2);//debug
		return vaddr_read(val2,4);
	}
}
uint32_t eval(int p,int q,bool *ok){
        *ok=true;
        
	if(p>q)
	{
		//printf("xxxx%d \n",q);
		//assert(0);
		*ok=false;
		return 0;
	}
	else if(p==q)
	{
	/*	uint32_t num=0;
		int j=0;
		while(tokens[p].str[j]!='\0')
		{//存在溢出问题
			int temp=tokens[p].str[j]-'0';
			num=num*10+temp;
			j++;
		}
		return num;
	*/
		return 	evalnum(p,ok);

	}
	else if(check_parentheses(p,q)==true)
	{
		return eval(p+1,q-1,ok);
	}
	else
	{
		int op=-1;
		int kuohao=0;//识别括号
		bool isand=false;//有无&&
		bool islow=false;//有无==
		bool ismid=false;//有无+-
		bool ishigh=false;//有无*/
		for(int i=p;i<=q;i++)
		{
			switch(tokens[i].type)
			{
				case TK_AND:
				{	
					if(kuohao==0)
					{
						if(op<i)op=i;
						isand=true;
						islow=true;
					}
					break;
				}
				case TK_EQ:
				{
					if(kuohao==0&&!isand)
					{
						if(op<i)op=i;
						islow=true;
					
					}
					break;
				}
				case'+':
				{
					if(!islow&&kuohao==0)
					{
						if(op<i)op=i;	
						ismid=true;
					}
					break;
				}
				case'-':
				{
					if(!islow&&kuohao==0)
					{
						if(op<i)op=i;
						ismid=true;
					}
					break;
				}
				case'*':
				{
					if(kuohao==0&&!islow&&!ismid)
					{
						if(op<i)op=i;
						ishigh=true;

					}
					break;
				}
				case'/':
				{
					if(kuohao==0&&!islow&&!ismid)
					{
						if(op<i)op=i;
						ishigh=true;
					}
					break;
				}
				case TK_DEREF:
				{
					if(kuohao==0&&!islow&&!ismid&&!ishigh)
					{
						if(op<i)op=i;
					}
					break;
				}
				
				case'(':
				{
					kuohao++;
					break;
				}
				case')':
				{
					kuohao--;
					if(kuohao<0)
					{
						printf("wrong\n");
						assert(0);	//报错
					}

					break;
				}
				default:break;

			}
		}
		printf("op%d\n",op);//ddebug		
		bool isok1,isok2;
		int op_type=tokens[op].type;
                 printf("op%d\n",op_type);//ddebug		
		uint32_t val1=eval(p,op-1,&isok1);
		uint32_t val2=eval(op+1,q,&isok2);
		
		if(!isok2){
			*ok=false;
			return 0;
			}
		if(isok1)
		{
			return solve2(val1,op_type,val2,ok);
		}
		else
		{
			return solve1(op_type,val2,ok);
		}
/*
		int op_type=tokens[op].type;

		switch(op_type){
			case'+':return val1+val2;
			case'-':return val1-val2;
			case'*':return val1*val2;
			case'/':
				{if(val2==0){

					printf("出现除0\n");
					assert(0);
					    }
					return val1/val2;
				}
			default:
				printf("optype wrong/n");
				assert(0);
		}
		//主要要做的
*/
	}

}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for(int i=0;i<nr_token;i++)
  {
  	if(tokens[i].type=='*')
  	{
  		if(i==0)
  			tokens[i].type=TK_DEREF;
  		else if(tokens[i-1].type!=TK_NUM&&tokens[i-1].type!=TK_REG&&tokens[i-1].type!=TK_HEX&&tokens[i-1].type!=')')
  			tokens[i].type=TK_DEREF;
  	}
  }
  			
  /* TODO: Insert codes to evaluate the expression. */
  //TODO();
  bool flag=true;
  printf("%u",eval(0,nr_token-1,&flag));
  return eval(0,nr_token-1,&flag);
}

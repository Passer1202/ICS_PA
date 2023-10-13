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
const int MAX_EXR=120;//生成表达式不要太长
const int MAX_NUM=10086;//生成数字最大

const int spacelenmax=5;
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";
static int num_k=0;//括号数
static int index_buf=0;
static int choose(int n){
	return rand()%n;
}
static void gen_num(){
	int num=choose(MAX_NUM);//num大小可调
	sprintf(buf+index_buf,"%d",num);
	while(buf[index_buf])
	{
		index_buf++;
	}
}

static void gen(char x)
{
	sprintf(buf+index_buf,"%c",x);
	index_buf++;

}

static void gen_rand_space()
{
	int len=choose(spacelenmax);
	for(int i=0;i<len;i++)
	{
		buf[index_buf++]=' ';	
	}
	
}

static void gen_rand_op()
{
	int op=choose(4);
	switch(op)
	{
		case 0:gen('+');break;
		case 1:gen('-');break;
		case 2:gen('*');break;
		case 3:gen('/');break;
		       
	}
}
static void gen_rand_expr() {

	int ch=0;
	if(index_buf<MAX_EXR)
	{
		ch=choose(3);
	}
		
	switch(ch){
		case 0:gen_num();break;
		case 1:gen('(');gen_rand_expr();gen(')');break;
		default:gen_rand_expr();gen_rand_op();gen_rand_expr();
      // buf[index_buf]='\0'; 
       //	buf[0] = '\0';
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
    index_buf=0;
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
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

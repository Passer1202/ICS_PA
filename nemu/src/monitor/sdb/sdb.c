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
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"
#include <memory/vaddr.h>
static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_si(char *args){
	int step_num=0;//the number of step
	if(args==NULL)
		step_num=1;
	else
		sscanf(args,"%d",&step_num);
	cpu_exec(step_num);
	return 0;
}

static int cmd_info(char *args){
	if(strcmp(args,"r")==0)
	{
		isa_reg_display();
	}
/*	else if(strcmp(args,"w")==0)
	{
		sdb_watchpoint_display();
	}
	*/
	return 0;
}
static int cmd_x(char *args){//扫描物理内存
	/*求读取长度*/
	char* len=strtok(NULL," ");
	int length=0;
	sscanf(len,"%d",&length);
	/*求起始点，第一阶段简化为不需要表达式求值*/
	char* startpoint=strtok(NULL," ");
	paddr_t startp=0;
	sscanf(startpoint,"%x",&startp);
	
	const int width=4;
	for(int i=0;i<length;i++)
	{
		printf("0x%x	0x%x\n",startp,vaddr_read(startp,width));
		startp=startp+width;
	
	}
	return 0;

}

static int cmd_p(char* args){
	if(args==NULL){
		printf("empty input\n");
		return 0;
	}

	bool success=false;
	word_t ans=expr(args,&success);
	if(!success) assert(0);
	printf("%u\n",ans);
	return 0;
}
static int cmd_q(char *args) {
  nemu_state.state=NEMU_QUIT;	
  return -1;
}
/*随机测试用*/

static void test_expr()
{
	FILE *fp=fopen("/home/hengji/ics2023/nemu/tools/gen-expr/input","r");
	if(fp==NULL)perror("test_expr error");

	char *e=NULL;
	word_t correct_res;
	size_t len=0;
	ssize_t read;
	bool success=false;
	while(true){
		if(fscanf(fp,"%u",&correct_res)==-1)break;
		read=getline(&e,&len,fp);
		e[read-1]='\0';

		word_t res = expr(e,&success);

		assert(success);
		if(res!=correct_res){
			puts(e);
			printf("exected:%u,got:%u\n",correct_res,res);
			assert(0);
		}
	}
	fclose(fp);
	if(e) free(e);
	Log("expr test pass");
}

static int cmd_help(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si", "Single-step execution",cmd_si},
  {"info","Display relevant status/information",cmd_info},
  {"x","Scanning Memory",cmd_x},
  {"p","Expression evaluation",cmd_p}, 
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();
  /*测试expr*/
  test_expr();
  /* Initialize the watchpoint pool. */
  init_wp_pool();
}

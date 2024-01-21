#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

const int MAX_FILE_NAME=128;

static char name_buf[MAX_FILE_NAME];

static char arg_buf[MAX_FILE_NAME];

static void sh_handle_cmd(const char *cmd) {
	if(cmd!=NULL){
		if (strncmp(cmd, "echo", 4) == 0) 
			(strlen(cmd) == 5)?sh_printf("\n"):sh_printf("%s", cmd + 5);
    		else if (strlen(cmd) > MAX_FILE_NAME){
            		sh_printf("command invalid: longer than MAX_FILE_NAME!\n");
            		return;
            	}
            	
            	//下面对通过空格分开来argv的cmd进行解析
            	//argc<=2:
            	
            	int index=0;
            	int mark=0;
            	
            	//找出来分隔的空格
            	
            	while(cmd[index]!=' '){
            		
            		//没找到空格
            		if(cmd[index]=='\n'){
            			mark=1;
            			break;
            		}
            		index++;
            	
            	}
            	
            	for(int i=0;i<MAX_FILE_NAME;i++){
            	
            		name_buf[i]=0;
            		arg_buf[i]=0;
            		
            	}
            	
            	if(mark==1){
            		
            		int len=0;
            		char* p_cmd=(char*)cmd;
		    	while(len<strlen(cmd)-1)//删除换行符'\n'
		    	{
		    		name_buf[len++]=*p_cmd++;
		    	}
            		char *argv[] = {name_buf, NULL};
            		//assert(0);
            		printf("s",name_buf);
            		execve(name_buf, argv, NULL);
            
            	}
            	else{
            		
            		for(int i=0;i<index;i++){
            			name_buf[i]=cmd[i];
            		}
            		char* p_cmd=(char*)cmd+index+1;
            		for(int i=0;i<strlen(cmd)-index;i++){
            			arg_buf[i]=*p_cmd++;
            		}
            		
            		char *argv[] = {name_buf, arg_buf, NULL};
            		execve(name_buf, argv, NULL);
            	}
            	
            	
            	
            	
            	//char* p_name_buf;
            	/*
            	for(int i=0;i<MAX_FILE_NAME;i++)
            	{
            		name_buf[i]=0;
            	}
            	
            	char* p_cmd=(char*)cmd;
            	
            	int len=0;
            	while(len<strlen(cmd)-1)//删除换行符'\n'
            	{
            		name_buf[len++]=*p_cmd++;
            	}
            	
            	execvp(name_buf, NULL);
            	*/
            	
            	
            	
        }
	
	return;
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

 //assert(setenv("PATH", "/bin", 0) == 0);
 //PA4.1临时去掉
  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}

#include <NDL.h>
#include <SDL.h>
#include<assert.h>


#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
//assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {

  //unsigned size_of_buf=32;
  //printf("%d\n",malloc(32));
  char event_buf[32];
  
  
  //assert(0);
  if(NDL_PollEvent(event_buf,32)==1){
  //读取有事件
  //assert(0);
  	if(strncmp(event_buf,"ku",2)==0){
  		ev->key.type=SDL_KEYUP;
  		//assert(0);
  	}
  	else if(strncmp(event_buf,"kd",2)==0){
  		ev->key.type=SDL_KEYDOWN;
  		
  	}
  	//else assert(0);//确保没有其他事件
  	
  	assert(0);
  	int mark=0;
  	uint32_t i=0;
  	uint32_t bound=sizeof(keyname) / sizeof(keyname[0]);
  	
  	
  	for(;i<bound;i++){
  		
  		if(strncmp(event_buf + 3, keyname[i], strlen(event_buf) - 4) != 0||strlen(keyname[i])!=strlen(event_buf)-4) continue;//偏移量或待调试
  		mark=1;
  		ev->key.keysym.sym = i;
  		//assert(0);
  	}
  	
  	
  	
  	return 1;
  	
  }
  
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
	//assert(0);
	char wait_buf[32];
	while(NDL_PollEvent(wait_buf,32)==0);
	
	if(strncmp(wait_buf,"ku",2)==0)
		event->key.type=SDL_KEYUP;
	else
		event->key.type=SDL_KEYDOWN;
	
	int mark=0;
	
	uint32_t i=0;
  	uint32_t bound=sizeof(keyname) / sizeof(keyname[0]);
  	
  	for(;i<bound;i++){
  		
  		if(strncmp(wait_buf + 3, keyname[i], strlen(wait_buf) - 4) != 0||strlen(keyname[i])!=strlen(wait_buf)-4) continue;//偏移量或待调试
  		mark=1;
  		event->key.keysym.sym = i;
  		
  	}
	return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
//assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
//assert(0);
  return NULL;
}

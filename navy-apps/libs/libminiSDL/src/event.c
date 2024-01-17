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

  uint32_t size_of_buf=32;
  char *event_buf = (char *)malloc(size_of_buf * sizeof(char));
  
  //assert(0);
  if(NDL_PollEvent(event_buf,size_of_buf)==1){
  //读取有事件
  	if(strncmp(event_buf,"ku",2)==0){
  		ev->key.type=SDL_KEYUP;
  	}
  	else if(strncmp(event_buf,"kd",2)==0){
  		ev->key.type=SDL_KEYDOWN;
  	}
  	//else assert(0);//确保没有其他事件
  	
  	assert(0);
  	uint32_t i=0;
  	uint32_t bound=sizeof(keyname) / sizeof(keyname[0]);
  	
  	for(;i<bound;i++){
  		
  		if(strncmp(event_buf + 3, keyname[i], strlen(event_buf) - 4) != 0) continue;//偏移量或待调试
  		
  		ev->key.keysym.sym = i;
  	}
  	
  	free(event_buf);
  	
  	return 1;
  	
  }
  else{
  //未读取有事件
  	ev->key.keysym.sym = 0;
  	ev->key.type = SDL_USEREVENT; 
  
  }
  //assert(0);
  //free(event_buf);
  //assert(0);
  
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  SDL_PollEvent(event);
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

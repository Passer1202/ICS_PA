#include <NDL.h>
#include <sdl-timer.h>
#include <stdio.h>

SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
  return NULL;
}

int SDL_RemoveTimer(SDL_TimerID id) {
//assert(0);
  return 1;
}

uint32_t SDL_GetTicks() {
//assert(0);
	return NDL_GetTicks();
 
}

void SDL_Delay(uint32_t ms) {
//assert(0);
}

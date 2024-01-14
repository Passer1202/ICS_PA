#include <common.h>
#include "syscall.h"

int sys_yield() {
    yield();
    return 0;
}

void sys_exit(int x) {
    halt(x);
}

int sys_write(int fd, void *buf, size_t cnt){
    if(fd!=1&&fd!=2)return -1;//调用失败
    char* out=buf;
    size_t num=cnt;
    while(num--){
    	putch(*out++);
    }
    
    return cnt;
}


void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  
  switch (a[0]) {
    case SYS_yield: c->GPRx=sys_yield(); break;
    case SYS_exit:  sys_exit(c->GPR2); break;
    case SYS_write: c->GPRx=sys_write(c->GPR2,(void*)c->GPR3,(size_t)c->GPR4);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
 
  
}

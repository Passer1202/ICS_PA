#include <common.h>
#include "syscall.h"

int sys_yield() {
    yield();
    return 0;
}

void sys_exit(int x) {
    halt(x);
}

int sys_brk(void* addr){
	return 0;
}

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  switch (a[0]) {
    case SYS_yield: c->GPRx=sys_yield(); break;
    case SYS_exit:  sys_exit(c->GPR2); break;
    case SYS_write: c->GPRx=fs_write(c->GPR2,(void*)c->GPR3,(size_t)c->GPR4);break;
    case SYS_brk:   c->GPRx=sys_brk((void*)c->GPR2);break;
    case SYS_open:  c->GPRx=fs_open((const char*)c->GPR2,c->GPR3,c->GPR4);break;
    case SYS_read:  assert(0);c->GPRx=fs_read(c->GPR2,(void*)c->GPR3,(size_t)c->GPR4);break;
    case SYS_close: c->GPRx=fs_close(c->GPR2);break;
    case SYS_lseek: c->GPRx=fs_lseek(c->GPR2,(size_t)c->GPR3,c->GPR4);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
 
  
}

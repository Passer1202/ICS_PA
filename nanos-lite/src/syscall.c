#include <common.h>
#include "syscall.h"

//为获取时间，定义结构体，参考（搬运）自man 2 gettimeofday中定理
struct timeval{
	uint64_t tv_sec;	/*seconds*/
	uint64_t tv_usec; /*microseconds*/
};

struct timezone{
	int tz_minuteswest; /*minutes west of Greenwich*/
	int tz_dsttime;     /*type of DST correction*/
};

int sys_gettimeofday(struct timeval *tv, struct timezone *tz){
	
	uint64_t time_get=io_read(AM_TIMER_UPTIME).us;
	uint64_t one_millon=1000000;
	
	tv->tv_sec=time_get/one_millon;//秒
	tv->tv_usec=time_get-(tv->tv_sec)*one_millon;//微秒
	
	
	return 0;
}

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
    case SYS_read:  c->GPRx=fs_read(c->GPR2,(void*)c->GPR3,(size_t)c->GPR4);break;
    case SYS_close: c->GPRx=fs_close(c->GPR2);break;
    case SYS_lseek: c->GPRx=fs_lseek(c->GPR2,(size_t)c->GPR3,c->GPR4);break;
    case SYS_gettimeofday: 
    		    c->GPRx=sys_gettimeofday((struct timeval *)c->GPR2,(struct timezone *)c->GPR3);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
 
  
}

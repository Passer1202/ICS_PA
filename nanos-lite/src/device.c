#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

//类似数电键盘实验，我们建立一个缓冲区;
#define MAX_BUF 32
static char key_buf[MAX_BUF];


static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
	//忽略掉了offset:字符设备
	for(size_t i=0;i<len;i++){
		putch(*((char *)buf+i));
	}
       	return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
	
	//初始化先清空缓冲区
	for(int i=0;i<MAX_BUF;i++){
		key_buf[i]=0;
	}
	
	//io读取事件
	AM_INPUT_KEYBRD_T my_event=io_read(AM_INPUT_KEYBRD);
	
	
	//不是none继续
	if(my_event.keycode!=AM_KEY_NONE){
	
		char* key_name=(char*)keyname[my_event.keycode];
		
		int len_get;
		if(my_event.keydown)len_get=sprintf(key_buf,"kd %s\n",key_name);
		else len_get=sprintf(key_buf,"ku %s\n",key_name);
		
		len_get=(len_get>=len)?len:len_get;//最长写入n字节

		for(int i=0;i<len_get;i++){
			char* p_buf=(char*)buf;
			char* p=key_buf;
			*p_buf++=*p++;
		}
		return len_get;
	}
	
	//*(char*)buf='\0';
	return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}

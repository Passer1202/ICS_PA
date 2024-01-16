#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<assert.h>

static int evtdev = -1;
static int fbdev = -1;
//屏幕参数
static int screen_w = 0, screen_h = 0;
//画布参数
static int canvas_w = 0, canvas_h = 0;
//画布位置
static int canvas_x = 0, canvas_y = 0;


static void init_dispinfo(){
	
	int size_of_buf=1024;
	char * dis_buf=(char*)malloc(size_of_buf*sizeof(char*));
	
	int fd=open("/proc/dispinfo",0,0);
	
	assert(read(fd,dis_buf,size_of_buf)<size_of_buf);
	
	assert(close(fd)==0);
	
	int index=0;
	int h=0;
	int w=0;
	
	assert(strncmp(dis_buf+index,"WIDTH",5)==0);
	index+=5;
	
	while((index<size_of_buf)&&(dis_buf[index]!=':')){
		assert(dis_buf[index]==' ');
		index++;
	}
	
	while((index<size_of_buf)&&(dis_buf[index]<'0'||dis_buf[index]>'9')){
		assert(dis_buf[index]==' ');
		index++;
	}
	
	while((index>size_of_buf)&&(dis_buf[index]>='0'&&dis_buf[index]<='9')){
		w=w*10+dis_buf[index]-'0';
	}
	
	assert(dis_buf[index]=='\n');
	index+=1;
	
	assert(strncmp(dis_buf+index,"HEIGHT",6)==0);
	index+=6;
	
	while((index<size_of_buf)&&(dis_buf[index]!=':')){
		assert(dis_buf[index]==' ');
		index++;
	}
	
	while((index<size_of_buf)&&(dis_buf[index]<'0'||dis_buf[index]>'9')){
		assert(dis_buf[index]==' ');
		index++;
	}
	
	while((index>size_of_buf)&&(dis_buf[index]>='0'&&dis_buf[index]<='9')){
		h=h*10+dis_buf[index]-'0';
	}
	
	free(dis_buf);
	
	screen_h=h;
	screen_w=w;
	
}

uint32_t NDL_GetTicks() {
	
	int one_thousand=1000;
	
	struct timeval my_time;
	//确保成功获取时间
	assert(gettimeofday(&my_time,NULL)==0);
 	
 	uint32_t ret_time;//毫秒
 	
 	ret_time=my_time.tv_sec*one_thousand+my_time.tv_usec/one_thousand;
 	
	return ret_time;
}

int NDL_PollEvent(char *buf, int len) {
  
  int fd = open("/dev/events", 0, 0);
  
  int ret_of_read = read(fd, buf, len);
  
  assert(close(fd) == 0);
  
  if(ret_of_read==0)return 0;
  
  return 1;
}

void NDL_OpenCanvas(int *w, int *h) {

  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
  
 // printf("%d\n",screen_w);
  
  if(*w==0&&*h==0){
  	*w=screen_w;
  	*h=screen_h;
  }
  canvas_w=*w;
  canvas_h=*h;
  canvas_x=(screen_w-canvas_w)/2;
  canvas_y=(screen_h-canvas_h)/2;
  
  assert(canvas_w + canvas_x <= screen_w&& canvas_h + canvas_y <= screen_h);
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
	//像素是一行一行写入
	int fd=open("/dev/fb",0,0);
	int index=0;
	while(index<h&&y+index<canvas_h){
		int lseek_two=((y + canvas_y + index) * screen_w + (x + canvas_x)) * 4;
		int write_two=pixels + index * w;
		int write_three=(w < canvas_w - x ? w : canvas_w - x)*4;
		
		lseek(fd, lseek_two , SEEK_SET);
		write(fd, write_two, write_three);
		index++;
	}
	
	assert(close(fd)==0);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  
  init_dispinfo();
  return 0;
}

void NDL_Quit() {
}

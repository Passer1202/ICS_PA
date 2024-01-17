#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

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
	
	//printf("%d\n",fd);
	assert(read(fd,dis_buf,size_of_buf)<size_of_buf);
	
	
	assert(close(fd)==0);
	
	int i=0;
	int h=0;
	int w=0;
	//printf("%s\n",dis_buf);
	assert(strncmp(dis_buf+i,"WIDTH",5)==0);
	
	i+=5;
	
	while((i<size_of_buf)&&(dis_buf[i]!=':')){
		assert(dis_buf[i]==' ');
		i++;
	}
	i++;
	
	while((i<size_of_buf)&&(dis_buf[i]<'0'||dis_buf[i]>'9')){
		
		assert(dis_buf[i]==' ');
		i++;
	}
	
	while((i<size_of_buf)&&(dis_buf[i]>='0'&&dis_buf[i]<='9')){
		w=w*10+dis_buf[i]-'0';
		i++;
	}
	
	assert(dis_buf[i]=='\n');
	i+=1;
	
	assert(strncmp(dis_buf+i,"HEIGHT",6)==0);
	i+=6;
	
	while((i<size_of_buf)&&(dis_buf[i]!=':')){
		assert(dis_buf[i]==' ');
		i++;
	}
	i++;
	while((i<size_of_buf)&&(dis_buf[i]<'0'||dis_buf[i]>'9')){
		assert(dis_buf[i]==' ');
		i++;
	}
	
	while((i<size_of_buf)&&(dis_buf[i]>='0'&&dis_buf[i]<='9')){
		h=h*10+dis_buf[i]-'0';
		i++;
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
  
   //assert(0);
  memset(buf, 0, len);
  
   //assert(0);
  int fd = open("/dev/events", 0, 0);
   //assert(0);
  int ret_of_read = read(fd, buf, len);
  
   //assert(0);
  close(fd);
  //assert(0);
  //为了让event能在native上跑，忍痛注释掉
  //assert(close(fd) == 0);
  
  //assert(0);
  //printf("%d\n",ret_of_read);
  if(ret_of_read==0)return 0;
  //assert(0);
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
   init_dispinfo();
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
	int i=0;
	while(i<h&&y+i<canvas_h){
		//开始定义为int native不出图像，后来在改称size_t ok了
		size_t lseek_two=((y + canvas_y + i) * screen_w + (x + canvas_x)) * 4;
		size_t write_two=pixels + i * w;
		size_t write_three=(w < canvas_w - x ? w : canvas_w - x)*4;
		
		lseek(fd, lseek_two , SEEK_SET);
		write(fd, write_two, write_three);
		i++;
	}
	
	//assert(close(fd)==0);
	//为了在native中能够翻页，忍痛删掉close
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
  
  return 0;
}

void NDL_Quit() {
}

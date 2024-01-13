#include <am.h>
#include <nemu.h>
#include<stdio.h>


#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
 /*
  int i;
  int w = io_read(AM_GPU_CONFIG).width ; 
  int h = io_read(AM_GPU_CONFIG).height;  
  
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = i;
  outl(SYNC_ADDR, 1);
 */ 
}

//static const int vga_mask=0xffff;
void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  
  //uint32_t information=inl(VGACTL_ADDR);
  uint32_t h=300;
  uint32_t w=400;
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = w * h * sizeof(uint32_t)
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {

  uint32_t wid=400;
  int32_t w = ctl->w, h = ctl->h;
  if (!ctl->sync && (w == 0 || h == 0))
    return;
  int32_t x = ctl->x, y = ctl->y;
  uint32_t *mypixels = ctl->pixels;
  uint32_t *p_fb = (uint32_t *)(uintptr_t)FB_ADDR;
  
  for (int i = y; (i < y+h); i++) {
    for (int j = x; (j < x+w); j++) {
    if(wid*i+j>=10){printf("wid:%d i:%d j:%d x:%d y:%d h:%d w:%d\n",wid,i,j,x,y,h,w);yield();}
      p_fb[wid*i+j] = mypixels[w*(i-y)+(j-x)]; 
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);    
  }
  
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

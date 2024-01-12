#include <am.h>
#include <nemu.h>
//#include<stdio.h>
#include<string.h>

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
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {

  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // uint16_t W = (inl(VGACTL_ADDR) & 0xff00) >> 16;
  // uint16_t H = inl(VGACTL_ADDR) & 0x00ff;
  uint16_t W = 400;
  uint16_t H = 300;

  int x = ctl->x;
  int y = ctl->y;
  int w = ctl->w;
  int h = ctl->h;
  uint32_t * base = (uint32_t *) ctl->pixels;
  int cp_bytes = sizeof(uint32_t) * (w < W - x ? w : W - x);
  for (int j = 0; j < h && y + j < H; ++j) {
      memcpy(&fb[(y + j) * W + x], base, cp_bytes);
      base += w;
  }
  //for (int j = 0; j < h; ++j)
  //  for (int i = 0; i < w; ++i)
  //    fb[(y + j) * W + x + i] = base[j * w + i];
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
  
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

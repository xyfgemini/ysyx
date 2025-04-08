#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)


void __am_gpu_init() {
  // int i;
  // int w = inw(VGACTL_ADDR + 2);  
  // int h = inw(VGACTL_ADDR); 
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // for (i = 0; i < w * h; i ++) fb[i] = i;
  // outl(SYNC_ADDR, 1);
}


void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true,
    .has_accel = false,
    .width = inw(VGACTL_ADDR + 2), 
    .height = inw(VGACTL_ADDR),
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x = ctl->x;
  int y = ctl->y;
  int w = ctl->w;
  int h = ctl->h;
  // if(!ctl->sync && (w == 0 || h == 0)){
  //   return;
  // }
  int k = 0;
  uint32_t *pixels = ctl->pixels;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t width = inw(VGACTL_ADDR + 2); //width:400
  // uint32_t height = inw(VGACTL_ADDR); //height:300
  for(int j=y;j<y+h;j++){
    for(int i=x;i<x+w;i++){
      fb[width*j+i] = pixels[k++];
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  //敲下释放按键，CPU read 键盘码,没有按键时，NONE

  kbd->keydown = inl(KBD_ADDR) ;
  kbd->keycode =  ;

}



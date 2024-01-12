#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
 // kbd->keydown = 0;
 // kbd->keycode = AM_KEY_NONE;
  
  int32_t keycode = inl(KBD_ADDR);
  kbd->keydown = (keycode & KEYDOWN_MASK) ? 1 : 0;
  kbd->keycode = (keycode & (~KEYDOWN_MASK));
}

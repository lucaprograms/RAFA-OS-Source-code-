#include <stdint.h>
#include "drivers.h"

int k11 = 0;
int d11 = 0;


void debugsettings(uint32_t* framebuffer, uint32_t width, uint32_t height, uint32_t pitch)
{
  clear_screen(background, framebuffer, width, height, pitch);
  draw_string("[e] Enable debug mode", 50, 50, textcolor, framebuffer, pitch);
  draw_string("[d] Disable debug mode", 50, 110, textcolor, framebuffer, pitch);
  while (1)
  {
    char key110 = get_hardware_keypress();
    if (key110 != 0)
    {
      if (key110 == 'e')
      {
        d11 = 1;
        k11 = 1;
        break;
      } else {
        d11 = 0;
        k11 = 0;
        break;
      }
    }
  }
}
int returnk(void)
{
  return k11;
}
int returnd(void)
{
  return d11;
}

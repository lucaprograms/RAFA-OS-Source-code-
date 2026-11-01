#include <stdint.h>
#include "drivers.h"

int k11 = 0;
int d11 = 0;

int k12 = 0;
void debugsettings(uint32_t* framebuffer, uint32_t width, uint32_t height, uint32_t pitch)
{
  draw_string("[e] Enable debug mode", 50, 50, textcolor, framebuffer, pitch);
  draw_string("[d] Disable debug mode", 50, 110, textcolor, framebuffer, pitch);
    draw_string("[c] Enable calc app access from main menu mode", 50, 160, textcolor, framebuffer, pitch);
  draw_string("[m] Disable calc app access from main menu mode", 50, 210, textcolor, framebuffer, pitch);
 draw_string("[h] Enable halloween mode", 50, 260, textcolor, framebuffer, pitch);
 draw_string("[i] Set textcolor to white", 50, 310, textcolor, framebuffer, pitch);
draw_string("[r] Set textcolor to red", 50, 360, textcolor, framebuffer, pitch);
  draw_string("[u] Set textcolor to yellow", 50, 410, textcolor, framebuffer, pitch);
  draw_string("[g] Set textcolor to gray", 50, 460, textcolor, framebuffer, pitch);
  draw_string("[p] Set textcolor to cyan", 50, 510, textcolor, framebuffer, pitch);

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
      } else if (key110 == 'd') {
        d11 = 0;
        k11 = 0;
        break;
      }
      else if (key110 == 'c')
      {
        k12 = 1;
        break;
      }
      else if (key110 == 'm')
      {
        k12 = 0;
        break;
      }
      if (key110 == 'r')
      {
        textcolor = 0xFFFF0000;
        break;
      }
      if (key110 == 'u')
      {
        textcolor = yellow;
        break;
      }
      if (key110 == 'i')
      {
        textcolor = 0xFFFFFF;
        break;
      }
      if (key110 == 'g')
      {
        textcolor = grey;
        break;
      }
      if (key110 == 'p')
      {
        textcolor = 0xFF00FFFF;
        break;
      }
      if (key110 == 'h')
      {
        textcolor = 0xFFFF6600;
        break;
      }
      else
      {
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
int returnk1(void)
{
  return k12;
}

#include "drivers.h"
#include <stdint.h>

void proggrammingt(uint32_t* framebuffer, uint32_t width, uint32_t height, uint32_t pitch)
{
  draw_string("Rust is a strong typed language (based on C).", 50, 50, textcolor, framebuffer, pitch);
  draw_string("C is a common proggramming language.", 50, 70, textcolor, framebuffer, pitch);
  draw_string("C# is a proggramming language commonly used in Windows.", 50, 90, textcolor, framebuffer, pitch);
  draw_string("C++ is an more object oreintented proggramming language (based on C)", 50, 110, textcolor, framebuffer, pitch);
  draw_string("Learn more at: github.com/lucaprograms/proggrammingstartertutorial", 50, 130, textcolor, framebuffer, pitch);
  draw_string("Press any key to exit...", 50, 160, textcolor, framebuffer, pitch);
  while (1)
  {
    if (get_hardware_keypress() != 0)
    {
      break;
    }
  }
}

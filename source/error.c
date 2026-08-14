#include "drivers.h"
#include <stdint.h>

void darkness(void)
{
  yellow = 0xFFFF00U;
  background = 0x0000000;
  textcolor = 0xFFFFFF;
  grey = 0xAAAAAAU;

}

void kernel_error(uint32_t* framebuffer, uint32_t width, uint32_t height, uint32_t pitch, int canreboot)
{
                darkness();
                clear_screen(background, framebuffer, width, height, pitch);
                draw_string("Kernel Error", center_text_x(width, "Kernel Error"), center_text_y(height),  0x00A00000, framebuffer, pitch);
                delay_ms_software(500000);
                if (canreboot == 1)
                {
                  reboot();
                }
}

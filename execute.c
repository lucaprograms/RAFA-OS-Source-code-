
#include <stdint.h>
#include "drivers.h"

uint32_t *framebufferstored;
uint32_t pitchstored;

void setf(uint32_t *framebuffer1, uint32_t pitch)
{
    framebufferstored = framebuffer1;
    pitchstored = pitch;
}

void con(
    uint64_t condition,
    const char *str,
    uint32_t x,
    uint32_t y,
    uint32_t color,
    uint32_t pitch,
    uint32_t a1
)
{
    if (condition == 1) {
        draw_string(
            str,
            x,
            y,
            color,
            framebufferstored,
            pitchstored
        );
    }
  if (condition == 2)
  {
    // con 2 [number of values to be changed] [value 1] [value 2] [value 3] [value 4] ..
    // return value -1 = invalid number of values to be changed.
    /*
      background = 0xFFFF00U;
  textcolor = 0x000000U;
  yellow = 0x00B8860B;
  grey = 0x00333333;
    */
    if (x == 1)
    {
        background = y;
    } else if (x == 2)
    {
      background = y;
      textcolor = color;
    }
    else if (x == 3)
    {
      background = y;
      textcolor = color;
      yellow = pitch;
    }
    else if (x == 4)
    {
      background = y;
      textcolor = color;
      yellow = pitch;
      grey = a1;
    }
    else {
    
    __asm__ volatile (
      "movq $-1, %%rax"
      :
      :
      : "rax"
    );
    }
  }
    if (condition == 3) {
        __asm__ volatile (
            "movq $1, %%rax"
            :
            :
            : "rax"
        );

        while (1) {
            char key1010 = get_hardware_keypress();

            if (key1010 != 0)
                break;
        }

        __asm__ volatile (
            "movq $0, %%rax"
            :
            :
            : "rax"
        );
    }

    if (condition == 4 &&
        (uintptr_t)str == 123 &&
        x == 321)
    {
        reboot();
    }

    if (condition == 5)
    {
    if (x == 0)
    {
        set_software_brightness(0.9);
    } else
  {
set_software_brightness1(0.9);
    }
    }
   if (condition == 6)
  {
    char input_char = get_hardware_keypress();
__asm__ volatile (
        "" // No instructions needed if input is forced into %al
        : 
        : [in_val] "a" (input_char) // "a" forces input_char directly into AL/RAX
        : // no clobber needed here
    );
  }
      if (condition == 7)
  {
    char input_char;
    while (1)
    {
      char i10 = get_hardware_keypress();
      if (i10 != 0)
      {
        input_char = i10;
        break;
      }
    }
__asm__ volatile (
        "" // No instructions needed if input is forced into %al
        : 
        : [in_val] "a" (input_char) // "a" forces input_char directly into AL/RAX
        : // no clobber needed here
    );
  }
  if (condition == 8)
  {
    uint32_t input_char = textcolor;

__asm__ volatile (
    "movl %0, %%eax"
    :
    : "r"(input_char)
    : "eax"
);
  }
}

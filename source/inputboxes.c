#include <stdint.h>
#include <efi.h>
#include <efilib.h>
#include "drivers.h"
#include "inputboxes.h"
int enableDiagonal = 1;
static char s[100000];
static int tracker = 0;
static int tr = 0;
char* returnmessage()
{
  char* msg;
  if (enableDiagonal == 1)
  {
    msg = "[n] start a new line \n[t] press the tab key [c] Clear the screen [d] Diagonal mode [x] Hide menu. Press any other key to exit inputboxes...";
  } else
  {
    msg = "[n] start a new line \n [h] Hide Diagonal Mode [t] press the tab key [c] Clear the screen [x] Hide menu. Press any other key to exit inputboxes...";
  }
  return msg;

}
int inputboxes(uint32_t* framebuffer, uint32_t width, uint32_t height, uint32_t pitch)
{
  clear_screen(background, framebuffer, width, height, pitch);
  char key;
  char key1;
  int count = 0;
  int x = Val + 4;
  int y = Val + 4;
  int breaksig = 0;
  int code = 0;
  int x1 = Val + 4;
  int y1 = Val + 4;
  while (1)
  {
  key = get_hardware_keypress();
  if (key != 0)
  {
    if (key == '\n' || key == '\r')
    {
      draw_string(returnmessage(),100, 700, textcolor, framebuffer, pitch);
      while (1)
      {
        key1 = get_hardware_keypress();
        if (key1 != 0)
        {
          if (key1 == 'n' || key1 == 'N')
          {
            breaksig = 2;
            y += Val;
            x = Val + 4;
                    // 1. Coordinate alignment (Matching your draw_string at X=100, Y=400)
uint32_t start_x = 100;
uint32_t start_y = 700;

// 2. Dynamic bounding box calculation (71 characters * font width)
uint32_t end_x = start_x + (71 * Val);
uint32_t end_y = start_y + Val + 4; // Font height plus a small vertical cushion

// 3. Clean the background using your existing draw_pixel function
for (uint32_t y = start_y; y < end_y; y++) 
{
    for (uint32_t x = start_x; x < end_x; x++) 
    {
        draw_pixel(x, y, background, framebuffer, pitch);
    }
}
            break;
          } else if (key1 == 't' || key1 == 'T')
            {
               x += (4 * Val);
               breaksig = 2;
              uint32_t start_x = 100;
uint32_t start_y = 700;

// 2. Dynamic bounding box calculation (71 characters * font width)
uint32_t end_x = start_x + (71 * Val);
uint32_t end_y = start_y + Val + 4; // Font height plus a small vertical cushion

// 3. Clean the background using your existing draw_pixel function
for (uint32_t y = start_y; y < end_y; y++) 
{
    for (uint32_t x = start_x; x < end_x; x++) 
    {
        draw_pixel(x, y, background, framebuffer, pitch);
    }
}
               break;
            } else if (key1 == 'd' || key1 == 'D')
            {
              if (enableDiagonal == 1)
              {
                clear_screen(background, framebuffer, width, height, pitch);
              breaksig = 2;
                for (int i = 0; i < tracker; i++)
                {
                  draw_char(s[i], x1, y1, textcolor, framebuffer, pitch);
                  x1 += Val;
                  y1 += Val;
                }
              x1 = Val + 4;
              y1 = Val + 4;
              break;
              }
            } else if (key1 == 'x' || key1 == 'X')
            {
              breaksig = 2;
                            uint32_t start_x = 100;
uint32_t start_y = 700;

// 2. Dynamic bounding box calculation (71 characters * font width)
uint32_t end_x = start_x + (71 * Val);
uint32_t end_y = start_y + Val + 4; // Font height plus a small vertical cushion

// 3. Clean the background using your existing draw_pixel function
for (uint32_t y = start_y; y < end_y; y++) 
{
    for (uint32_t x = start_x; x < end_x; x++) 
    {
        draw_pixel(x, y, background, framebuffer, pitch);
    }
}
              break;
            } else if (key1 == 'c' || key1 == 'C')
            {
              breaksig = 2;
              x = Val + 4;
              y = Val + 4;
              clear_screen(background, framebuffer, width, height, pitch);
              break;
            }
            else if (key1 == 'h' || key1 == 'H')
            {
              breaksig = 2;
              if (enableDiagonal == 1)
              {
                enableDiagonal = 0;
              } else
              {
                enableDiagonal = 1;
              }
              break;
            }
            else {
            breaksig = 1;
              break;
          }
        }
        for (volatile int i = 0; i < 50000; i++) {}
      }
    }
    if (breaksig == 1)
    {
        break;
    }
    if (breaksig == 2)
    {
      breaksig = 0;
      continue;
    }
    if (key == '\t')
    {
      x += (4 * Val);
      continue;
    }
    draw_char(key, x, y, textcolor, framebuffer, pitch);
    tracker++;
    s[tracker] = key;
    count++;
    x += Val;
  }
    for (volatile int i = 0; i < 50000; i++);
  }
  clear_screen(background, framebuffer, width, height, pitch);
  return code;
}

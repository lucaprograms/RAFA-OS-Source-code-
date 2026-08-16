#include "main.h"
#include "error.h"
#include "debug.h"
#include "drivers.h"
#include "calc.h"
#include <efi.h>
#include <efilib.h>
#include <stdint.h>
#include "help.h"
#include "inputboxes.h"
#include "clock.h"
#include "resource.h"
#include "usb_driver.h"
#include <string.h>
#include "execute.h"

int d = 1;
EFI_RUNTIME_SERVICES* RTafter = NULL;
void testinject(void)
{
  // Does nothing:

  __asm__ volatile("nop");

  // Temporary code goes here:
  /*
  Test 1:
  textcolor = 0xFFFF00;
  textcolor = 0x000000;
  green = 0x006400;
  Test 2:
  background = 0x0000FF00;
  textcolor = 0x00003300;
  Test 3:
  background = 0x00A00000;
  textcolor = 0x000000;
  grey = 0x00333333;
  d = 5;
  Test 4:
  background = 0xFFFF00U;
  textcolor = 0x000000U;
  yellow = 0x00B8860B;
  grey = 0x00333333;
  d = 6;
  Test 5:
  */
//  testinject1();

  
  // Does nothing:

  __asm__ volatile("nop");
}
char* returnver()
{
  return "10.9.3";
}
char* inttohex(unsigned int xyzab)
{
  static char out_buf[16];
  

    // Handle 0 explicitly
    if (xyzab == 0) {
        out_buf[0] = '0';
        out_buf[1] = '\0';
        return out_buf;
    }

    const char hex_chars[] = "0123456789ABCDEF";
    int idx = 0;
    int started = 0;

    // Digit 8 (bits 28..31)
    if (started || (xyzab >> 28) != 0) {
        out_buf[idx++] = hex_chars[(xyzab >> 28) & 0xF];
        started = 1;
    }
    // Digit 7 (bits 24..27)
    if (started || ((xyzab >> 24) & 0xF) != 0) {
        out_buf[idx++] = hex_chars[(xyzab >> 24) & 0xF];
        started = 1;
    }
    // Digit 6 (bits 20..23)
    if (started || ((xyzab >> 20) & 0xF) != 0) {
        out_buf[idx++] = hex_chars[(xyzab >> 20) & 0xF];
        started = 1;
    }
    // Digit 5 (bits 16..19)
    if (started || ((xyzab >> 16) & 0xF) != 0) {
        out_buf[idx++] = hex_chars[(xyzab >> 16) & 0xF];
        started = 1;
    }
    // Digit 4 (bits 12..15)
    if (started || ((xyzab >> 12) & 0xF) != 0) {
        out_buf[idx++] = hex_chars[(xyzab >> 12) & 0xF];
        started = 1;
    }
    // Digit 3 (bits 8..11)
    if (started || ((xyzab >> 8) & 0xF) != 0) {
        out_buf[idx++] = hex_chars[(xyzab >> 8) & 0xF];
        started = 1;
    }
    // Digit 2 (bits 4..7)
    if (started || ((xyzab >> 4) & 0xF) != 0) {
        out_buf[idx++] = hex_chars[(xyzab >> 4) & 0xF];
        started = 1;
    }
    // Digit 1 (bits 0..3)
    if (started || (xyzab & 0xF) != 0) {
        out_buf[idx++] = hex_chars[xyzab & 0xF];
    }

    out_buf[idx] = '\0';
    return out_buf;
 }
char* returnm()
{
  static char l[32];
  if (d == 0)
  {
    strcpy(l, "Amber OS ");
    strncat(l, returnver(), 7);
    return l;
  } else if (d == 2)
  {
    strcpy(l, "Lily OS ");
    strncat(l, returnver(), 7);
    return l;
  } else if (d == 3)
  {
    strcpy(l, "Lola OS ");
    strncat(l, returnver(), 7);
    return l;
  }
  else if (d == 4)
  {
    strcpy(l, "RAFA OS (Green Edition) ");
    strncat(l, returnver(), 7);
    return l; 
  } else if (d == 5)
  {
    strcpy(l, "RAFA OS (Red Edition) ");
    strncat(l, returnver(), 7);
    return l;
  }
  else if (d == 6)
  {
    strcpy(l, "RAFA OS (Yellow Edition) ");
    strncat(l, returnver(), 7);
    return l;
  }
  else
  {
    strcpy(l, "RAFA OS ");
    strncat(l, returnver(), 7);
    return l;
  }
}
char* re(int xyz)
{
  static char l[32];
  if (xyz == 0)
  {
    strcpy(l, "Amber OS ");
    strncat(l, returnver(), 7);
    return l;
  } else if (xyz == 2)
  {
    strcpy(l, "Lily OS ");
    strncat(l, returnver(), 7);
    return l;
  } else if (xyz == 3)
  {
    strcpy(l, "Lola OS ");
    strncat(l, returnver(), 7);
    return l;
  }
  else if (xyz == 4)
  {
    strcpy(l, "RAFA OS (Green Edition) ");
    strncat(l, returnver(), 7);
    return l;
  }
   else if (d == 5)
  {
    strcpy(l, "RAFA OS (Red Edition) ");
    strncat(l, returnver(), 7);
    return l;
  }
    else if (d == 6)
  {
    strcpy(l, "RAFA OS (Yellow Edition) ");
    strncat(l, returnver(), 7);
    return l;
  }
  else
  {
    return "RAFA OS 2.2.2";
  }
}
void light(void)
{
    d = 0;
    yellow = 0x00B8860B;
    textcolor = 0x0000000;
    background = 0xFFFFFF;
    grey = 0x00333333;
}
void dark(void)
{
  d = 1;
  yellow = 0xFFFF00U;
  background = 0x0000000;
  textcolor = 0xFFFFFF;
  grey = 0xAAAAAAU;

}
void t(void)
{
  d = 2;
  yellow = 0xFFFF00U;
  background = 0x008080;
  textcolor = 0xFFFFFF;
  grey = 0xAAAAAAU;
}
void l(void)
{
    d = 3;
    yellow = 0xFFFF00U;
    background = 0xDC7EB5;
    textcolor = 0x000000;
    grey = 0x00222222;
}
void g(void)
{
    d = 4;
    yellow = 0x00B8860B;
    background = 0x00FF00;
    textcolor = 0x000000;
    grey = 0x00333333;
}
char* inttobin(unsigned int a, char *out_buf) {
    if (!out_buf) return NULL;

    if (a == 0) {
        out_buf[0] = '0';
        out_buf[1] = '\0';
        return out_buf;
    }

    int idx = 0;
    int started = 0;

    // Check each bit from MSB (bit 31) to LSB (bit 0)
    for (int i = 31; i >= 0; i--) {
        int bit = (a >> i) & 1; // Extract single bit
        if (bit || started) {
            out_buf[idx++] = '0' + bit;
            started = 1;
        }
    }

    out_buf[idx] = '\0';
    return out_buf;
}
void red(void)
{
  background = 0x00A00000;
  textcolor = 0x000000;
  grey = 0x00333333;
  d = 5;
  yellow = 0xFFFF00U;
}
int test_ram_region(uint64_t base_address, uint64_t size_in_bytes) {
    volatile uint64_t* ptr = (volatile uint64_t*)base_address;
    uint64_t num_elements = size_in_bytes / sizeof(uint64_t);
    
    uint64_t test_pattern = 0xAA55AA55AA55AA55ULL;

    // 1. Write phase
    for (uint64_t i = 0; i < num_elements; i++) {
        ptr[i] = test_pattern;
    }

    // 2. Read and Verify phase
    for (uint64_t i = 0; i < num_elements; i++) {
        if (ptr[i] != test_pattern) {
            // Memory error detected at (base_address + (i * sizeof(uint64_t)))
            return -1; 
        }
    }

    // Optional: Test with inverted pattern
    test_pattern = ~test_pattern;
    for (uint64_t i = 0; i < num_elements; i++) { ptr[i] = test_pattern; }
    for (uint64_t i = 0; i < num_elements; i++) {
        if (ptr[i] != test_pattern) return -1;
    }

    return 0; // Region passed
}
int testhardware(uint32_t* framebuffer,
          uint32_t width,
          uint32_t height,
          uint32_t pitch)
{
if (test_ram_region(0x200000, 4096) != 0)
{
    return 1;
}
 uint32_t r = 255;
    uint32_t g = 0;
    uint32_t b = 0;

    while (1)
    {
    test_ram_region(0x200000, 4096);
        uint32_t color = (r << 16) | (g << 8) | b;

        clear_screen(color, framebuffer, width, height, pitch);
        if (get_hardware_keypress() != 0)
    {
      clear_screen(background, framebuffer, width, height, pitch);
      break;
    }

        delay_ms_software(1);

        // Red -> Yellow
        if (r == 255 && g < 255)
            g++;

        // Yellow -> Green
        else if (g == 255 && r > 0)
            r--;

        // Green -> Cyan
        else if (g == 255 && b < 255)
            b++;

        // Cyan -> Blue
        else if (b == 255 && g > 0)
            g--;

        // Blue -> Magenta
        else if (b == 255 && r < 255)
            r++;

        // Magenta -> Red
        else if (r == 255 && b > 0)
            b--;
    }
  return 0;
}
#include <stddef.h>

// Converts a binary string (e.g. "1001") to an unsigned int
unsigned int bintoint(const char *str) {
    if (!str) return 0;

    unsigned int result = 0;

    while (*str != '\0') {
        char c = *str;
        
        if (c == '0' || c == '1') {
            // Shift existing result left by 1 (multiply by 2) and add the bit
            result = (result << 1) | (c - '0');
        } else {
            // Stop at invalid binary character or null terminator
            break;
        }
        str++;
    }

    return result;
}

// Converts a hex string (e.g. "1A", "0x1A", or "1a") to an unsigned int
unsigned int hextoint(const char *str) {
    if (!str) return 0;

    unsigned int result = 0;

    // Optional: Skip "0x" or "0X" prefix if present
    if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) {
        str += 2;
    }

    while (*str != '\0') {
        char c = *str;
        unsigned int val = 0;

        if (c >= '0' && c <= '9') {
            val = c - '0';
        } else if (c >= 'A' && c <= 'F') {
            val = 10 + (c - 'A');
        } else if (c >= 'a' && c <= 'f') {
            val = 10 + (c - 'a');
        } else {
            // Stop at invalid hex character
            break;
        }

        // Shift existing result left by 4 bits (multiply by 16) and add digit value
        result = (result << 4) | val;
        str++;
    }

    return result;
}
char* inttostr(int num, char *out_buf) {
    if (!out_buf) return NULL;

    // Handle 0 explicitly
    if (num == 0) {
        out_buf[0] = '0';
        out_buf[1] = '\0';
        return out_buf;
    }

    int idx = 0;
    int is_negative = 0;
    unsigned int n;

    // Handle negative numbers
    if (num < 0) {
        is_negative = 1;
        n = (unsigned int)(-num); // Handles absolute value cleanly
    } else {
        n = (unsigned int)num;
    }

    char tmp[16];
    int ti = 0;

    // Extract decimal digits right-to-left
    while (n > 0 && ti < 15) {
        tmp[ti++] = '0' + (n % 10);
        n /= 10;
    }

    // Add negative sign if applicable
    if (is_negative) {
        out_buf[idx++] = '-';
    }

    // Reverse digits into destination buffer
    for (int i = 0; i < ti; i++) {
        out_buf[idx++] = tmp[ti - 1 - i];
    }

    out_buf[idx] = '\0';
    return out_buf;
}
void drawmenu(uint32_t* framebuffer, uint32_t width, uint32_t height, uint32_t pitch)
{
   clear_screen(background, framebuffer, width, height, pitch);
      draw_string(returnm(), 50, 50, textcolor, framebuffer, pitch);
    draw_string("Press 'h' to open the help app.", 50, 110, textcolor, framebuffer, pitch);
}
void yellow1(void)
{
    background = 0xFFFF00U;
  textcolor = 0x000000U;
  yellow = 0x00B8860B;
  grey = 0x00333333;
  d = 6;
}


#include <stdint.h>

static uint32_t seed = 12345;

void seed_random(char value)
{
    seed ^= (uint32_t)value;
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;
}

int maina(void)
{
    seed ^= seed << 13;
    seed ^= seed >> 17;
    seed ^= seed << 5;

    return (seed % 10) + '0';
}

int mainx(uint32_t* framebuffer,
          uint32_t width,
          uint32_t height,
          uint32_t pitch)
{
  int sig10 = 0;
  while (1)
  {
    char g101 = get_hardware_keypress();
    if (g101 == 'e')
    {
      sig10 = 1;
      break;
    }
    else if (g101 != 0)
    {
      break;
    }
  }
  if (sig10 == 1)
  {
    return 100;
  }
    clear_screen(background, framebuffer, width, height, pitch);

    char guess = 0;

    draw_string(
        "Guess a number (0-9): ",
        center_text_x(width, "Guess a number (0-9): "),
        center_text_y(height),
        textcolor,
        framebuffer,
        pitch
    );

    while (guess == 0)
    {
        guess = get_hardware_keypress();

        if (guess != 0)
        {
            seed_random(guess);
        }
    }

    int target = maina();

    if (guess == target)
    {
        draw_string(
            "Correct!",
            center_text_x(width, "Correct!"),
            50,
            textcolor,
            framebuffer,
            pitch
        );
    
        draw_char(
            target,
            center_text_x(width, "a"),
            110,
            textcolor,
            framebuffer,
            pitch
        );
     delay_ms_software(99999);
        clear_screen(background, framebuffer, width, height, pitch);
    }
    else
    {
        draw_string(
            "Wrong! The number was: ",
            center_text_x(width, "Wrong! The number was: "),
            50,
            textcolor,
            framebuffer,
            pitch
        );

        draw_char(
            target,
            center_text_x(width, "a"),
            110,
            textcolor,
            framebuffer,
            pitch
        );
    delay_ms_software(99999);
        clear_screen(background, framebuffer, width, height, pitch);
    }
  

    return 0;
}

// 1. Reserve a blank 64-byte staging area in safe memory

void kernel_main_after_efi(uint32_t* framebuffer, uint32_t width, uint32_t height, uint32_t pitch) {
    //__asm__ __volatile__("cli");
    // Use our new clean graphic functions from drivers.c
    init_drivers();
    setf(framebuffer, pitch);
  testinject();


  if (test_ram_region(0x200000, 4096) != 0)
  {
    kernel_error(framebuffer, width, height, pitch, 1);
  }
 
int x = 1;

if (x != 1)
{
    kernel_error(framebuffer, width, height, pitch, 1);
  }
/* if (background != 0x0000000)
{
    kernel_error(framebuffer, width, height, pitch, 0);
}*/


    
    init_framebuffer_ptr(framebuffer, width, height, pitch);
    // 3. Print confirmation to the screen
    handle_timezone_prompt(framebuffer, pitch);
  /*
   static char buf1[32];
   draw_string(inttostr(hextoint("FF"), buf1), 50, 50, textcolor, framebuffer, pitch);
  */
  drawmenu(framebuffer, width, height, pitch);

    int a = 0;
    int e = 1;
    int d1 = 0;
    int mk = 1;
static int last_second = -1;
while (1) {
    int hh, mm, ss;
    rtc_get_time(&hh, &mm, &ss);

    if (ss != last_second) {
        last_second = ss;
        draw_clock(framebuffer, pitch, 50, 200);
        int caps = capslock_state();
      for (uint32_t y = 70; y < 85; y++) {
        for (uint32_t x = 50; x < 250; x++) {
            draw_pixel(x, y, background, framebuffer, pitch);
        }
    }
        if (caps == 1)
        {
            
    draw_string("CAPS LOCK IS ON!", 50, 70, textcolor, framebuffer, pitch);  
        } else
        {
              
    draw_string("CAPS LOCK IS OFF!", 50, 70, textcolor, framebuffer, pitch);
        }
    }
    char pressed_key;
    if (mk == 0)
    {
        pressed_key = get_hardware_keypress();
    }
    else if (mk == 1)
  {
      pressed_key = '5';
      mk = 2;
    } else
  {
      clear_screen(background, framebuffer, width, height, pitch);
      pressed_key = '6';
      mk = 0;
    }
    
        
        if (pressed_key != 0) {
            // Draw a quick rectangle backdrop to wipe old character
            if (e == 1)
            {
        for (uint32_t y = 400; y < 400 + 16; y++) {
        for (uint32_t x = 50; x < 50 + 16; x++) {
            draw_pixel(x, y, background, framebuffer, pitch);
        }
    }
            }
            if (a == 1)
            {
              draw_char(pressed_key, 50, 400, textcolor, framebuffer, pitch); 
            }
            if (pressed_key == 'K' || pressed_key == 'k')
            {
            if (e == 1)
            {
              if (a == 1)
              {
                a = 0;
              } else
              {
                a = 1;
              }
            }
            }
            if (pressed_key == 'r' || pressed_key == 'R')
            {
              monitorapp(framebuffer, pitch); 
        drawmenu(framebuffer, width, height, pitch);
            }
            if (pressed_key == 'b' || pressed_key == 'b')
            {
              red();
                drawmenu(framebuffer, width, height, pitch);
            }
            if (pressed_key == '4')
            {
              if (d1 == 1)
              {
                kernel_error(framebuffer, width, height, pitch, 1);
              }
            }
            if (pressed_key == '3')
            {
              debugsettings(framebuffer, width, height, pitch);
              d1 = returnd();
              a = returnk();
                drawmenu(framebuffer, width, height, pitch);
            }
            if (pressed_key == 'v' || pressed_key == 'V')
            {
              yellow1();
                drawmenu(framebuffer, width, height, pitch);
            }
            if (pressed_key == 'h' || pressed_key == 'H')
            {
              noefi_help(framebuffer, width, height, pitch);
                   drawmenu(framebuffer, width, height, pitch);
            }
            if (pressed_key == 'i' || pressed_key == 'I')
            {
              inputboxes(framebuffer, width, height, pitch);
                drawmenu(framebuffer, width, height, pitch);
            }
            if (pressed_key == '8')
            {
                clear_screen(background, framebuffer, width, height, pitch);
                draw_string("Press any key to stop test..", center_text_x(width, "Press any key to stop test.."), center_text_y(height), textcolor, framebuffer, pitch);
                delay_ms_software(100000);
               testhardware(framebuffer, width, height, pitch);
              drawmenu(framebuffer, width, height, pitch);
            }
            if (pressed_key == '7')
            {
        while (1)
        {
            if (mainx(framebuffer, width, height, pitch) == 100)
          {
            drawmenu(framebuffer, width, height, pitch);
            break;
          }
        }
            }
      if (pressed_key == '9')
      {
        clear_screen(background, framebuffer, width, height, pitch);
        draw_string("CPU stress test is running. Press any key to stop stress test....", center_text_x(width, "CPU stress test is running. Press any key to stop stress test...."), center_text_y(height), textcolor, framebuffer, pitch);
        while (1)
        {
          test_ram_region(0x200000, 4096);
          if (get_hardware_keypress() != 0)
          {
            break;
          }
          delay_ms_software(10);
        }
        drawmenu(framebuffer, width, height, pitch);
      }
            if (pressed_key == '1')
            {
        clear_screen(background, framebuffer, width, height, pitch);
        draw_string("[1] Decimal to binafry.", 50, 50, textcolor, framebuffer, pitch);
        draw_string("[2] Decimal to hexadecimal.", 50, 60, textcolor, framebuffer, pitch);
        draw_string("[5] Exit", 50, 70, textcolor, framebuffer, pitch);
        draw_string("[6] Reload main menu.", 50, 80, textcolor, framebuffer, pitch);
        draw_string("Warning: You might have to press the key twice.", 50, 100, yellow, framebuffer, pitch);
        while (1)
        {
          char l4 = get_hardware_keypress();
          if (l4 != 0)
          {
            break;
          }
        }
        clear_screen(background, framebuffer, width, height, pitch);
              while (1)
              {
                char l = get_hardware_keypress();
                if (l == '1')
                {
            char l1 = 0;
            int num1 = 0;
            while (1)
            {
              l1 = get_hardware_keypress();
       if (l1 >= '0' && l1 <= '9') {
                int digit1 = l1 - '0';
                
                        num1 = num1 * 10.0 + digit1;

            }
              if (l1 == 'e')
              {
                break;
              }
            }
            static char buffer123[32];
            draw_string(inttobin(num1, buffer123), 50, 50, textcolor, framebuffer, pitch);
          }                  
              if (l == '5')
              {
                  drawmenu(framebuffer, width, height, pitch);
                break;
              }
              if (l == '6')
              {
                clear_screen(background, framebuffer, width, height, pitch);
                draw_string("[1] Decimal to binary.", 50, 50, textcolor, framebuffer, pitch);
                draw_string("[2] Decimal to hexadecimal.", 50, 60, textcolor, framebuffer, pitch);
                draw_string("[5] Exit", 50, 70, textcolor, framebuffer, pitch);
                draw_string("[6] Reload main menu.", 50, 80, textcolor, framebuffer, pitch);
                draw_string("Warning: You might have to press the key twice.", 50, 100, yellow, framebuffer, pitch);
                       while (1)
        {
          char l5 = get_hardware_keypress();
          if (l5 != 0)
          {
            break;
          }
        }
            clear_screen(background, framebuffer, width, height, pitch);
            continue;
              }

              if (l == '2')
              {
            char l1 = 0;
            int num1 = 0;
            while (1)
            {
              l1 = get_hardware_keypress();
       if (l1 >= '0' && l1 <= '9') {
                int digit1 = l1 - '0';
                
                        num1 = num1 * 10.0 + digit1;

            }
              if (l1 == 'e')
              {
                break;
              }
            }
                  
                  // number = l1 * 10 + l2
                  draw_string(inttohex(num1), 50, 50, textcolor, framebuffer, pitch);
                  l = get_hardware_keypress();  
              }
              }
            }
            if (pressed_key == 'x' || pressed_key == 'X')
            {
              clear_screen(background, framebuffer, width, height, pitch);
              int x3 = 20;
              int y3 = 0;
              int nosetx = 0;
              char l = get_hardware_keypress();
              while (1)
              {
             l = get_hardware_keypress();
          if (l != 0)
          {
                if (l == '1')
                {
                  x3 += 16;
                  l = get_hardware_keypress();
                  continue;
                }
                if (l == '2')
                {
                  y3 += 16;
                  l = get_hardware_keypress();
                  continue;
                }
                if (l == '3')
                {
                  x3 -= 16;
                  nosetx = 1;
                  l = get_hardware_keypress();
                  continue;
                }
                if (l == '4')
                {
                  y3 -= 16;
                  l = get_hardware_keypress();
                  continue;
                }
                if (l == '5')
                {
                  y3 += 16;
                  l = get_hardware_keypress();
                  nosetx = 1;
                  continue;
                }
                if (l == '6')
                {
                  y3 -= 16;
                  l = get_hardware_keypress();
                  nosetx = 1;
                  continue;
                }
                if (l == '7')
                {
                  break;
                }
                if (l == '8')
                {
                  x3 = 20;
                  y3 = 0;
                  nosetx = 0;
                  clear_screen(background, framebuffer, width, height, pitch);
                  l = get_hardware_keypress();
                  continue;
                }
                draw_char(l, x3, y3, textcolor, framebuffer, pitch);
                if (nosetx == 0)
                {
                x3 += 16;
                }
                nosetx = 0;
              }
        }
  drawmenu(framebuffer, width, height, pitch);
            }
            if (pressed_key == 'a' || pressed_key == 'A')
            {
              e = 1;
              a = 0;
                drawmenu(framebuffer, width, height, pitch);
            }
            if (pressed_key == 'c' || pressed_key == 'C')
            {
               int x = minimal_calc(framebuffer, pitch);
	       if (x == 100)
	       {
	      	e = 1;
              	  drawmenu(framebuffer, width, height, pitch);
           testinject();
	       }
            }
            if (pressed_key == 'd' || pressed_key == 'D')
            {
                      e = 1;
              a = 0;
              dark();
                drawmenu(framebuffer, width, height, pitch);
         testinject();
            }
            if (pressed_key == 'u' || pressed_key == 'U')
            {
                      e = 1;
              a = 0;
              l();
                drawmenu(framebuffer, width, height, pitch);
         testinject();
            }
            if (pressed_key == 'l' || pressed_key == 'L')
            {
                      e = 1;
              a = 0;
              light();
                drawmenu(framebuffer, width, height, pitch);
         testinject();
            }
            if (pressed_key == 'y')
            {
              static char l[32];
              clear_screen(background, framebuffer, width, height, pitch);
              draw_string(returnm(), center_text_x(width, returnm()), 50, textcolor, framebuffer, pitch);
              strcpy(l, "Respectively RAFA OS ");
              strncat(l, returnver(), 7);
              draw_string(l, center_text_x(width, l), 400, textcolor, framebuffer, pitch);
              char keypressed1 = get_hardware_keypress();
              while (1)
              {
                keypressed1 = get_hardware_keypress();
                if (keypressed1 != 0)
                {
                  break;
                }
              }
                        drawmenu(framebuffer, width, height, pitch);
         testinject();
            }
            if (pressed_key == 'g' || pressed_key == 'G')
            {
                      e = 1;
              a = 0;
              g();
                drawmenu(framebuffer, width, height, pitch);
         testinject();
            }
if (pressed_key == '2')
{
  set_software_brightness(0.9);
}
if (pressed_key == '0')
{
set_software_brightness1(0.9);        
}
      static char password;
      static char password1;
      static char password2;
      static char password3;
if (pressed_key == '5')
      {
        clear_screen(background, framebuffer, width, height, pitch);
         draw_string("Set your 4 character password: ", center_text_x(width, "Set your 4 character password: "), center_text_y(height), textcolor, framebuffer, pitch);
        while (1)
        {
          password = get_hardware_keypress();
          if (password != 0)
          {
            break;
          }
        }
        while (1)
        {
          password1 = get_hardware_keypress();
          if (password1 != 0)
          {
            break;
          }
        }
        while (1)
        {
          password2 = get_hardware_keypress();
          if (password2 != 0)
          {
            break;
          }
        }
        while (1)
        {
          password3 = get_hardware_keypress();
          if (password3 != 0)
          {
            break;
          }
        }
         drawmenu(framebuffer, width, height, pitch);
      }
      if (pressed_key == '6')
      {
        clear_screen(background, framebuffer, width, height, pitch);
        draw_string("RAFA OS login screen", center_text_x(width, "rafa os login screen"), 50, textcolor, framebuffer, pitch);
        char g = 0;
        while (1) {
    g = get_hardware_keypress();
    int hh, mm, ss;
    rtc_get_time(&hh, &mm, &ss);

    if (ss != last_second) {
        last_second = ss;
                        for (uint32_t y = 400U; y < 415U; y++) {
    for (uint32_t x = 550U; x < 750U; x++) {
        draw_pixel(x, y, background, framebuffer, pitch);
    }
}
        draw_clock(framebuffer, pitch, center_text_x(width, "12:34"), center_text_y(height));
    }
    if (g != 0)
    {
      break;
    }
}
        clear_screen(background, framebuffer, width, height, pitch);
        draw_string("Type in the password: ", center_text_x(width, "Type in the password :"), center_text_y(height), textcolor, framebuffer, pitch);
        while (1)
        {
          char keyinput = get_hardware_keypress();
          if (keyinput == password)
          {
            break;
          }
        }
        while (1)
        {
           char keyinput = get_hardware_keypress();
          if (keyinput == password1)
          {
            break;
          }
        }
        while (1)
        {
           char keyinput = get_hardware_keypress();
          if (keyinput == password2)
          {
            break;
          }
        }
        while (1)
        {
           char keyinput = get_hardware_keypress();
          if (keyinput == password3)
          {
            break;
          }
        }
         drawmenu(framebuffer, width, height, pitch);
      }
            if (pressed_key == 'o' || pressed_key == 'O')
            {
                      e = 1;
              a = 0;
              t();
                drawmenu(framebuffer, width, height, pitch);
         testinject();
            }
            if (pressed_key == 't')
            {
        // y = width / 2;
        // x = height / 2;
                      clear_screen(background, framebuffer, width, height, pitch);
              static int last_second = -1;
            draw_string(returnm(), center_text_x(width, returnm()), 50, textcolor, framebuffer, pitch);
char g = get_hardware_keypress();
while (1) {
    g = get_hardware_keypress();
    int hh, mm, ss;
    rtc_get_time(&hh, &mm, &ss);

    if (ss != last_second) {
        last_second = ss;
                        for (uint32_t y = 400U; y < 415U; y++) {
    for (uint32_t x = 550U; x < 750U; x++) {
        draw_pixel(x, y, background, framebuffer, pitch);
    }
}
        draw_clock(framebuffer, pitch, center_text_x(width, "12:34"), center_text_y(height));
    }
    if (g != 0)
    {
      break;
    }
}
                     drawmenu(framebuffer, width, height, pitch);
         testinject();
            }
            if (pressed_key == 'p' || pressed_key == 'P')
            {
              clear_screen(background, framebuffer, width, height, pitch);
              draw_string("Press 'r' to reboot this PC. Press any other key to exit..", 50, 50, textcolor, framebuffer, pitch);
              draw_string("", 50, 110, textcolor, framebuffer, pitch);
              char poweroff = get_hardware_keypress();
              while (1)
              {
                poweroff = get_hardware_keypress();
                if (poweroff != 0)
                {
                if (poweroff == 'r' || poweroff == 'R')
                {
                  reboot();
                }
                else
                {
                  break;
                }
             for (volatile int i = 0; i < 50000; i++);
                }
              }
                drawmenu(framebuffer, width, height, pitch);
         testinject();
            }
    }

        for (volatile int i = 0; i < 50000; i++);
    }
}
EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    ST->ConOut->SetAttribute(ST->ConOut, EFI_TEXT_ATTR(EFI_LIGHTGRAY, EFI_BLACK));
    ST->ConOut->ClearScreen(ST->ConOut);
    
    EFI_INPUT_KEY Key;
    EFI_STATUS Status;

    Print(L"=========================================\n");
    Print(L"         RAFA OS 10.9.3 BOOTLOADER         \n");
    Print(L"=========================================\n\n");
    Print(L"  [r] Reboot your PC\n");
    Print(L"  [s] Shutdown your PC\n");
    Print(L"  [c] Continue to RAFA OS\n");
    Print(L"  [u] Boot to UEFI Firmware\n");
    Print(L"  [q] Quit\n");
    Print(L"  [h] Help\n\n");

    ST->ConIn->Reset(ST->ConIn, FALSE);

    // --- SETUP THE 1-SECOND TICK TIMER ---
    EFI_EVENT TickTimer;
    UINTN SecondsRemaining = 7;
    
    // 1 second = 10,000,000 in 100ns units
    Status = uefi_call_wrapper(BS->CreateEvent, 5, EVT_TIMER, TPL_CALLBACK, NULL, NULL, &TickTimer);
    if (!EFI_ERROR(Status)) {
        uefi_call_wrapper(BS->SetTimer, 3, TickTimer, TimerPeriodic, 10000000);
    }

    while (TRUE)
    {
        // 1. Update live countdown string on the screen
        if (SecondsRemaining > 0) {
            Print(L"\rCountinuing to RAFA OS automatically in %d seconds...   ", SecondsRemaining);
        } else {
            Print(L"\rCountinuing to RAFA OS now...                           \n");
            Key.UnicodeChar = L'c'; // Auto-force the continue block
            goto launch_kernel;
        }

        // 2. Check if a 1-second interval has ticked down
        Status = uefi_call_wrapper(BS->CheckEvent, 1, TickTimer);
        if (Status == EFI_SUCCESS) {
            if (SecondsRemaining > 0) {
                SecondsRemaining--;
            }
        }

        // 3. Non-blocking check for actual user keypresses
        Status = ST->ConIn->ReadKeyStroke(ST->ConIn, &Key);
        if (Status == EFI_SUCCESS) 
        {  
            // If they pressed anything, we can stop the text countdown
            uefi_call_wrapper(BS->SetTimer, 3, TickTimer, TimerCancel, 0);

launch_kernel:
            if (Key.UnicodeChar == L'r' || Key.UnicodeChar == L'R')
            {
                uefi_call_wrapper(BS->CloseEvent, 1, TickTimer);
                ST->ConOut->ClearScreen(ST->ConOut);
                RT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
            }
            else if (Key.UnicodeChar == L's' || Key.UnicodeChar == L'S' || Key.ScanCode == SCAN_ESC)
            {
                uefi_call_wrapper(BS->CloseEvent, 1, TickTimer);
                ST->ConOut->ClearScreen(ST->ConOut);
                RT->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
            }
            else if (Key.UnicodeChar == L'u' || Key.UnicodeChar == L'U')
            {
                uefi_call_wrapper(BS->CloseEvent, 1, TickTimer);
                EFI_GUID GlobalVarGuid = EFI_GLOBAL_VARIABLE;
                UINT64 indications = EFI_OS_INDICATIONS_BOOT_TO_FW_UI;

                RT->SetVariable(L"OsIndications", &GlobalVarGuid,
                                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                                sizeof(indications), &indications);
                RT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
            }
            else if (Key.UnicodeChar == L'h' || Key.UnicodeChar == L'H') 
            {
                efi_help();
                // 3. Clean up the help UI and redraw your clean boot menu screen
                ST->ConOut->ClearScreen(ST->ConOut);
                
                Print(L"=========================================\n");
                Print(L"          RAFA OS 10.9.3 BOOTLOADER         \n");
                Print(L"=========================================\n\n");
                Print(L"  [r] Reboot your PC\n");
                Print(L"  [s] Shutdown your PC\n");
                Print(L"  [c] Continue to RAFA OS\n");
                Print(L"  [u] Boot to UEFI Firmware\n");
                Print(L"  [q] Quit\n");
                Print(L"  [h] Help\n\n");

                // 4. Reset the 30-second countdown timer so they have time to read/decide
                SecondsRemaining = 7;
                uefi_call_wrapper(BS->SetTimer, 3, TickTimer, TimerPeriodic, 10000000);
            }
            else if (Key.UnicodeChar == L'q' || Key.UnicodeChar == L'Q')
            {
                uefi_call_wrapper(BS->CloseEvent, 1, TickTimer);
                break;
            }
            // THIS handles both an intentional 'c' press AND the auto-boot timeout!
            else if (Key.UnicodeChar != 0)
            {
                uefi_call_wrapper(BS->CloseEvent, 1, TickTimer);
                ST->ConOut->ClearScreen(ST->ConOut);
                
                EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
                EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
                if (EFI_ERROR(uefi_call_wrapper(BS->LocateProtocol, 3, &gop_guid, NULL, (void**)&gop))) return EFI_LOAD_ERROR;
                RTafter = SystemTable->RuntimeServices;
                uint32_t* fb_base = (uint32_t*)gop->Mode->FrameBufferBase;
                uint32_t width   = gop->Mode->Info->HorizontalResolution;
                uint32_t height  = gop->Mode->Info->VerticalResolution;
                uint32_t pitch   = gop->Mode->Info->PixelsPerScanLine;

                UINTN memory_map_size = 0, map_key = 0, descriptor_size = 0;
                uint32_t descriptor_version = 0;
                EFI_MEMORY_DESCRIPTOR *mem_map_buf = NULL;

                uefi_call_wrapper(BS->GetMemoryMap, 5, &memory_map_size, NULL, &map_key, &descriptor_size, &descriptor_version);
                memory_map_size += (4 * descriptor_size);
                if (EFI_ERROR(uefi_call_wrapper(BS->AllocatePool, 3, EfiLoaderData, memory_map_size, (void **)&mem_map_buf))) return EFI_LOAD_ERROR;
                if (EFI_ERROR(uefi_call_wrapper(BS->GetMemoryMap, 5, &memory_map_size, mem_map_buf, &map_key, &descriptor_size, &descriptor_version))) return EFI_LOAD_ERROR;

                Status = uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, map_key);         
        if (Status != EFI_SUCCESS) {
                Print(L" Boot Error. Trying agian\n\n");
          uefi_call_wrapper(BS->Stall, 1, 5000000);
                    uefi_call_wrapper(BS->GetMemoryMap, 5, &memory_map_size, mem_map_buf, &map_key, &descriptor_size, &descriptor_version);
                    if (EFI_ERROR(uefi_call_wrapper(BS->ExitBootServices, 2, ImageHandle, map_key))) {Print(L"Boot Error. Exiting..\n\n"); uefi_call_wrapper(BS->Stall, 1, 5000000);return EFI_LOAD_ERROR;}
                }

                kernel_main_after_efi(fb_base, width, height, pitch);
            }
        }
        else if (Status == EFI_NOT_READY)
        {
            uefi_call_wrapper(BS->Stall, 1, 4000); // Small delay to prevent CPU spinning aggressively
        }
    }
    return EFI_SUCCESS;
}


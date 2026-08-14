#include <stdint.h>
#include "drivers.h"
#include "clock.h"

int isam = 1;
static uint8_t cmos_read(uint8_t reg) {
    outb(0x70, reg);
    return inb(0x71);
}

static int rtc_update_in_progress() {
    outb(0x70, 0x0A);
    return inb(0x71) & 0x80;
}

static uint8_t bcd_to_bin(uint8_t val) {
    return (val & 0x0F) + ((val >> 4) * 10);
}

void rtc_get_time(int* hour, int* minute, int* second) {
    while (rtc_update_in_progress());

    uint8_t sec  = cmos_read(0x00);
    uint8_t min  = cmos_read(0x02);
    uint8_t hr   = cmos_read(0x04);
    uint8_t statusB = cmos_read(0x0B);

    int is_binary = statusB & 0x04;   // bit 2: 1 = binary, 0 = BCD

    if (!is_binary) {
        sec  = bcd_to_bin(sec);
        min  = bcd_to_bin(min);
        hr   = bcd_to_bin(hr);
    }

    *second = sec;
    *minute = min;
    *hour   = hr;
}

// --------------------
// Timezone handling
// --------------------

static int timezone_offset = 0;   // hours offset from RTC
static int last_second = -1;

void set_timezone_offset(int offset) {
    timezone_offset = offset;
}

static void apply_timezone(int* hour) {
    int h = *hour + timezone_offset;
    h %= 24;
    if (h < 0) h += 24;
    *hour = h;
}

// --------------------
// Drawing the clock
// --------------------
static void format_time1(char* buf, int hh, int mm, int ss) {
    buf[0] = '0' + (hh / 10);
    buf[1] = '0' + (hh % 10);
    buf[2] = ':';
    buf[3] = '0' + (mm / 10);
    buf[4] = '0' + (mm % 10);
    buf[5] = 0;
}
static void format_time(char* buf, int hh, int mm, int ss) {
    // 1. Determine AM vs PM
    int is_pm = (hh >= 12);

    // 2. Convert 24-hour to 12-hour format (1 to 12)
    int hh12 = (hh % 12 == 0) ? 12 : (hh % 12);

    // Format HH:MM:SS
    buf[0] = '0' + (hh12 / 10);
    buf[1] = '0' + (hh12 % 10);
    buf[2] = ':';
    buf[3] = '0' + (mm / 10);
    buf[4] = '0' + (mm % 10);
    
    // Add space and AM/PM tag
    buf[5]  = ' ';
    buf[6]  = is_pm ? 'P' : 'A';
    buf[7] = 'M';
    buf[8] = '\0';
}

void draw_clock(uint32_t* fb, uint32_t pitch, uint32_t x, uint32_t y) {
    int hh, mm, ss;
    rtc_get_time(&hh, &mm, &ss);
    apply_timezone(&hh);
  /*int isam = 1;
while (1)
  {
    char isampm = get_hardware_keypress();
    if (isampm != 0)
    {
      if (isampm == '1')
      {
        isam = 1;
        break;
      }
      else
      {
        isam = 0;
        break;
      }
    }
  }*/
    // clear area
    // 11 characters @ 8px wide = 88px total width
for (uint32_t cy = y; cy < y + 16; cy++) {
    for (uint32_t cx = x; cx < x + 88; cx++) {
        draw_pixel(cx, cy, background, fb, pitch);
    }
}
    char buf[32];
    if (isam == 1)
    {
      format_time(buf, hh, mm, ss);
    } else
    {
      format_time1(buf, hh, mm, ss);
    }
    draw_string(buf, x, y, textcolor, fb, pitch);
}

// --------------------
// Timezone prompt
// --------------------
static char wait_for_keypress() {
    char k = 0;
    while (k == 0) {
        k = get_hardware_keypress();
    }
    return k;
}
static void wait()
{
while (1)
  {
    char isampm = get_hardware_keypress();
    if (isampm != 0)
    {
      if (isampm == '0')
      {
        isam = 0;
        break;
      }
      else
      {
        isam = 1;
        break;
      }
    }
  }
}


void handle_timezone_prompt(uint32_t* fb, uint32_t pitch) { 
    draw_string("Timezone offset (+/- hours):", 50, 230, textcolor, fb, pitch);
    int sign = 1;
    int value = 0;

    char k = wait_for_keypress();

    if (k == '-') {
        sign = -1;
        k = wait_for_keypress();
    } else if (k == '+') {
        k = wait_for_keypress();
    }

    while (k >= '0' && k <= '9') {
        value = value * 10 + (k - '0');
        k = wait_for_keypress();
    }

    timezone_offset = sign * value;
    draw_string("Set is am/pm format: defualt 1: ", 50, 250, textcolor, fb, pitch);
    wait();
    for (uint32_t cy = 250; cy < 250 + 16; cy++) {
    for (uint32_t cx = 50; cx < 50 + 256; cx++) {
        draw_pixel(cx, cy, background, fb, pitch);
    }
}

    draw_string("Timezone set.", 50, 250, green, fb, pitch);
}
// --------------------
// Main clock update
// --------------------

void update_clock_loop(uint32_t* fb, uint32_t pitch) {
    int hh, mm, ss;
    rtc_get_time(&hh, &mm, &ss);
    apply_timezone(&hh);

    if (ss != last_second) {
        last_second = ss;
        draw_clock(fb, pitch, 50, 200);
    }

    char key = get_hardware_keypress();
    if (key == 't' || key == 'T') {
        handle_timezone_prompt(fb, pitch);
    }
}


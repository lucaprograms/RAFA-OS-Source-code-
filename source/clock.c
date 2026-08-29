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


void handle_timezone_prompt(uint32_t* fb, uint32_t pitch, uint32_t width, uint32_t height) { 
    draw_string("Enter the number based on the table (+/- hours):", 50, 230, textcolor, fb, pitch);
    const char* zones[] = {
    // Negative Offsets (West of UTC)
    "BIT -12", "SST -11", "NUT -11", "HAST -10",
    "HDT -9",  "AKST -9", "AKDT -8", "PST -8",
    "PDT -7",  "MST -7",  "MDT -6",  "CST -6",
    "CDT -5",  "EST -5",  "EDT -4",  "AST -4",
    "ADT -3",  "NST -3",  "NDT -2",  "BRT -3",
    "ART -3",  "FNT -2",  "GST -2",  "AZOT -1",
    "CVT -1",

    // Neutral Offsets (UTC / GMT)
    "UTC +0",  "GMT +0",  "WET +0",

    // Positive Offsets (East of UTC)
    "WEST +1", "BST +1",  "CET +1",  "CEST +2",
    "EET +2",  "EEST +3", "AST +3",  "MSK +3",
    "EAT +3",  "IRST +3", "IRDT +4", "GST +4",
    "AZT +4",  "MUT +4",  "AFT +4",  "PKT +5",
    "UZT +5",  "IST +5",  "NPT +5",  "BST +6",
    "KGT +6",  "MMT +6",  "WIB +7",  "ICT +7",
    "CST +8",  "SGT +8",  "AWST +8", "WST +8",
    "ULAT +8", "WITA +8", "JST +9",  "KST +9",
    "WIT +9",  "ACST +9", "ACDT +10", "AEST +10",
    "AEDT +11", "CHST +10", "VLAT +10", "SRET +11",
    "NFT +11", "NZST +12", "NZDT +13", "FJT +12",
    "PETT +12", "GILT +12", "PHOT +13", "TKT +13",
    "WST +13", "LINT +14", "TOST +14"
};

int total_zones = sizeof(zones) / sizeof(zones[0]);
int start_y = 250;
int line_height = 15;
int max_lines_per_column = 20; // Splits into 2 columns of 40 lines each
int column_width = 180;        // Space between column 1 and column 2

for (int i = 0; i < total_zones; i++) {
    // Determine column position (0 for left column, 1 for right column)
    int col = i / max_lines_per_column; 
    
    // Reset row index for the second column
    int row = i % max_lines_per_column; 

    int current_x = 50 + (col * column_width);
    int current_y = start_y + (row * line_height);

    draw_string(zones[i], current_x, current_y, textcolor, fb, pitch);
}
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
  clear_screen(background, fb, width, height, pitch);
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
    }
}


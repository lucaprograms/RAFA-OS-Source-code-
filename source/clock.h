#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

// -----------------------------
// RTC access
// -----------------------------

void rtc_get_time(int* hour, int* minute, int* second);

// -----------------------------
// Timezone support
// -----------------------------

void set_timezone_offset(int offset);
void handle_timezone_prompt(uint32_t* fb, uint32_t pitch);

// -----------------------------
// Clock drawing
// -----------------------------

void draw_clock(uint32_t* fb, uint32_t pitch, uint32_t x, uint32_t y);
void update_clock_loop(uint32_t* fb, uint32_t pitch);

#endif



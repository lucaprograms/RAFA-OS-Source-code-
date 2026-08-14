#ifndef DRIVERS_H
#define DRIVERS_H

#include <stdint.h>
#define PIT_COMMAND_PORT  0x43
#define PIT_CHANNEL_2     0x42
#define SPEAKER_PORT      0x61

// Colors
extern uint32_t background;
extern uint32_t textcolor;
extern uint32_t grey;
extern uint32_t green;
extern uint32_t yellow;

void init_drivers();
/* I/O port access */
uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);

/* Graphics */
void draw_pixel(uint32_t x, uint32_t y, uint32_t color,
                uint32_t* framebuffer, uint32_t pitch);

void clear_screen(uint32_t color, uint32_t* framebuffer,
                  uint32_t width, uint32_t height, uint32_t pitch);

void draw_char(char c, uint32_t x_pos, uint32_t y_pos,
               uint32_t color, uint32_t* framebuffer, uint32_t pitch);

void draw_string(const char* str, uint32_t x, uint32_t y,
                 uint32_t color, uint32_t* framebuffer, uint32_t pitch);
void draw_string_extra(const char* str, const char* str1, uint32_t x, uint32_t y,
                 uint32_t color, uint32_t* framebuffer, uint32_t pitch, int type);

/* Keyboard */
char get_hardware_keypress(void);
int capslock_state();

/* Power control */
void reboot(void);
void shutdown(void);

/* Sound & Delays */
void stop_speaker(void);
static inline void delay_ms_software(volatile uint32_t count)
{
    while (count--) {
        // Calibrate this value based on your CPU frequency and optimization level
        volatile uint32_t cycles = 4000; 
        while (cycles--) {
            __asm__("nop"); // Prevent loop optimization removal
        }
    }
}
void play_startup_chime(void); // <--- This will be your main boot sound function
uint32_t IsVirtualMachine(void);

static inline void set_pit_frequency(uint16_t divisor) {
    outb(PIT_COMMAND_PORT, 0xB6);
    outb(PIT_CHANNEL_2, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL_2, (uint8_t)((divisor >> 8) & 0xFF));
}

int center_text_x(uint32_t screen_width, const char *text);
int center_x(uint32_t screen_width, uint32_t object_width);

int center_y(uint32_t screen_height, uint32_t object_height);

int center_text_y(uint32_t screen_height);
/* --- Framebuffer & Software Brightness --- */

typedef struct {
    uint32_t *framebuffer;
    uint32_t width;
    uint32_t height;
    uint32_t pixels_per_scanline;
} SavedFB;

// Global instance so main.c can populate it before ExitBootServices()
extern SavedFB my_fb; 

int centerx(uint32_t width);
int centery(uint32_t height);
int utoint(uint32_t x);
// Software brightness controller (0.0f = black, 1.0f = full brightness)
void set_software_brightness(float factor);void init_framebuffer_ptr(uint32_t* fb, uint32_t w, uint32_t h, uint32_t pitch);

#endif

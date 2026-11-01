#include "drivers.h"

// Tracking variables for screen cursor coordinates
static uint32_t cursor_x = 50;
static uint32_t cursor_y = 130;

// Configuration definitions matching your graphic boundaries
#define SCREEN_WIDTH 1024
#define FONT_WIDTH   8
#define LINE_HEIGHT  12
#define LEFT_MARGIN  50

// Helper to check if a character marks the end of a word block
static int is_whitespace(char c) {
    return (c == ' ' || c == '\t' || c == '\n' || c == '\0');
}

// Counts the exact pixel width of an incoming word until the next space or format flag
static uint32_t get_word_width(const char* str) {
    uint32_t length = 0;
    while (*str && !is_whitespace(*str) && *str != '%') {
        length++;
        str++;
    }
    return length * FONT_WIDTH;
}

void kernel_printf(uint32_t* framebuffer, uint32_t pitch, uint32_t color, const char* format, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, format);

    const char* fmt_start = format;

    while (*format) {
        // --- Word Wrap Lookahead for Static Format Text ---
        if (*format != ' ' && *format != '%' && (format == fmt_start || is_whitespace(*(format - 1)))) {
            if (cursor_x + get_word_width(format) >= SCREEN_WIDTH) {
                cursor_x = LEFT_MARGIN;
                cursor_y += LINE_HEIGHT;
            }
        }

        if (*format == '%') {
            format++; // Look at the specifier flag
            
            if (*format == 's') { // --- STRINGS (%s) ---
                char* str = __builtin_va_arg(args, char*);
                char* str_start = str;
                
                while (*str) {
                    // Check dynamic text strings word-by-word
                    if (*str != ' ' && (str == str_start || is_whitespace(*(str - 1)))) {
                        if (cursor_x + get_word_width(str) >= SCREEN_WIDTH) {
                            cursor_x = LEFT_MARGIN;
                            cursor_y += LINE_HEIGHT;
                        }
                    }
                    draw_char(*str, cursor_x, cursor_y, color, framebuffer, pitch);
                    cursor_x += FONT_WIDTH;
                    str++;
                }
            } 
            else if (*format == 'c') { // --- CHARACTERS (%c) ---
                char c = (char)__builtin_va_arg(args, int);
                if (cursor_x + FONT_WIDTH >= SCREEN_WIDTH) {
                    cursor_x = LEFT_MARGIN;
                    cursor_y += LINE_HEIGHT;
                }
                draw_char(c, cursor_x, cursor_y, color, framebuffer, pitch);
                cursor_x += FONT_WIDTH;
            } 
            else if (*format == 'd') { // --- INTEGERS (%d) ---
                // Pulled safely as a full 64-bit value to prevent 0-value register misalignments
                uint64_t num = __builtin_va_arg(args, uint64_t);
                
                if (num == 0) {
                    if (cursor_x + FONT_WIDTH >= SCREEN_WIDTH) {
                        cursor_x = LEFT_MARGIN;
                        cursor_y += LINE_HEIGHT;
                    }
                    draw_char('0', cursor_x, cursor_y, color, framebuffer, pitch);
                    cursor_x += FONT_WIDTH;
                } else {
                    char digits[20]; // 20 slots perfectly accommodates largest uint64_t limits
                    int i = 0;
                    
                    while (num > 0) {
                        digits[i++] = '0' + (num % 10);
                        num /= 10;
                    }

                    // Wrap the entire integer block if it leaks past screen bounds
                    uint32_t num_width = i * FONT_WIDTH;
                    if (cursor_x + num_width >= SCREEN_WIDTH) {
                        cursor_x = LEFT_MARGIN;
                        cursor_y += LINE_HEIGHT;
                    }

                    // Flush array elements in forward order
                    while (i > 0) {
                        draw_char(digits[--i], cursor_x, cursor_y, color, framebuffer, pitch);
                        cursor_x += FONT_WIDTH;
                    }
                }
            }
            else if (*format == 'x') { // --- HEXADECIMAL CODES (%x) ---
                uint64_t hex_num = __builtin_va_arg(args, uint64_t);
                
                // Keep the '0x' header glued to the front of the text block safely
                if (cursor_x + (2 * FONT_WIDTH) >= SCREEN_WIDTH) {
                    cursor_x = LEFT_MARGIN;
                    cursor_y += LINE_HEIGHT;
                }
                draw_char('0', cursor_x, cursor_y, color, framebuffer, pitch); cursor_x += FONT_WIDTH;
                draw_char('x', cursor_x, cursor_y, color, framebuffer, pitch); cursor_x += FONT_WIDTH;

                if (hex_num == 0) {
                    draw_char('0', cursor_x, cursor_y, color, framebuffer, pitch);
                    cursor_x += FONT_WIDTH;
                } else {
                    char hex_digits[16];
                    int i = 0;
                    
                    while (hex_num > 0) {
                        uint64_t remainder = hex_num % 16;
                        hex_digits[i++] = "0123456789ABCDEF"[remainder];
                        hex_num /= 16;
                    }

                    // Wrap the entire hexadecimal block if space runs out
                    uint32_t hex_width = i * FONT_WIDTH;
                    if (cursor_x + hex_width >= SCREEN_WIDTH) {
                        cursor_x = LEFT_MARGIN;
                        cursor_y += LINE_HEIGHT;
                    }

                    while (i > 0) {
                        draw_char(hex_digits[--i], cursor_x, cursor_y, color, framebuffer, pitch);
                        cursor_x += FONT_WIDTH;
                    }
                }
            }
        } 
        else if (*format == '\n') { // --- MANUAL NEWLINE (\n) ---
            cursor_x = LEFT_MARGIN;
            cursor_y += LINE_HEIGHT;
        } 
        else { // --- STANDARD TEXT ALPHABETICS ---
            if (cursor_x + FONT_WIDTH >= SCREEN_WIDTH) {
                cursor_x = LEFT_MARGIN;
                cursor_y += LINE_HEIGHT;
            }
            draw_char(*format, cursor_x, cursor_y, color, framebuffer, pitch);
            cursor_x += FONT_WIDTH;
        }
        format++;
    }

    __builtin_va_end(args);
}

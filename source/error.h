#ifndef ERROR_H
#define ERROR_H

#include <stdint.h>


void kernel_error(uint32_t* framebuffer, uint32_t width, uint32_t height, uint32_t pitch, int canreboot);

#endif

#ifndef HELP_H
#define HELP_H

// Shared help list (array of strings)
extern const char *help_list[];
extern const int help_list_count;

// UEFI text-mode help (Print)
void efi_help(void);

// Kernel graphics-mode help (draw_string)
void noefi_help(uint32_t* framebuffer, uint32_t width, uint32_t height, uint32_t pitch);

#endif


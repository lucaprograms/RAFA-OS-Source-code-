#ifndef USB_DRIVER_H
#define USB_DRIVER_H

#include <stdint.h>

/**
 * @brief Scans the PCI bus for an xHCI (USB 3.0) controller, performs 
 * the BIOS-to-OS handoff, sets up memory-mapped I/O, allocates 
 * the required ring buffers, and kicks the controller into run mode.
 * * @return int 1 if initialization succeeded and the controller is online,
 * 0 if no xHCI controller was found or setup failed.
 */
int init_usb_subsystem(void);

/**
 * @brief Checks the xHCI Host Controller's Event Ring for incoming hardware packets.
 * If a valid USB HID Keyboard packet is parsed, it maps the scancode 
 * to ASCII and automatically triggers system actions (like 'd' for dark mode
 * and 'l' for light mode).
 * * @note This function should be executed continuously inside your kernel's main loop.
 */
char check_usb_keyboard(void);

#endif // USB_DRIVER_H

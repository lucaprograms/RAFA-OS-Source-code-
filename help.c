#include <efi.h>
#include <efilib.h>
#include "help.h"
#include "drivers.h"

const char *help_list[] = {
    "=========================================",
    "      RAFA OS 12.0.1 HELP       ",
    "=========================================",
    
    "Welcome to RAFA OS 12.0.1!",
    "Fun Fact: there is a second copy of the OS on the prepackged flashdrive (also includes old RAFA CALC OS (Just an efi file).",
    "Rafa OS 12.0.1 is an minmal one file kernel that includes a minmal calclator utillity.",
    "Make sure to vist my github website at: github.com/lucaprograms.",
    "Source code is at: https://github.com/lucaprograms/RAFA-OS-Source-code-",
    "Precompiled binary is at: https://github.com/lucaprograms/RAFA-OS-IN-ONE-EFI-FILE",
    "Once you get to the main os (kernel) menu, you can press certain keys to control and use the OS (kernel), such as: ",
    "",
    "[n]  Trigger Freeze System Error",
    "[m]  Open Math Interface",
    "[y]  Show the 'About your PC' screen.",
    "[i]  Open inputbox, where you can preview input",
    "[h]  Open this help menu",
    "[k]  Print which key is pressed (key debug mode)",
    "[a]  Redraw the main os (kernel) menu",
    "[p]  Power Options",
    "[t]  Show the clock",
    "[1]  Open number system conversion application",
    "[2]  Set brightness of screen color",
    "[3]  Open Settings",
    "[s]  Open Settings",
    "[4]  Trigger a Kernel Error. Debug mode is needed to be turned on.",
    "[5]  Set password",
    "[6]  Go to login screen",
    "[7]  Play guess random number game",
    "[8]  Launch Diagnostics application",
    "[&]  Open proggramming tutorial",
    "[*]  Play guess random two digit repeat number game",
    "[#]  Main Menu Style Screen Test",
    "[(]  Low Brightness Main Menu Redraw",
    "[)]  Extremely Low Brightness Main Menu Redraw",
    "=========================================\r\n",
    "Press any key to return to RAFA OS...\r\n"
};

const int help_list_count = sizeof(help_list) / sizeof(help_list[0]);

void efi_help(void)
{
	        EFI_INPUT_KEY Key;
		UINTN EventIndex;
                // Clear the loader screen cleanly
                ST->ConOut->ClearScreen(ST->ConOut);

    for (int i = 0; i < help_list_count; i++) {
        Print(L"%a\n", help_list[i]);
    }

                // Pause firmware thread natively until a key event register triggers
                ST->ConIn->Reset(ST->ConIn, FALSE);
                BS->WaitForEvent(1, &ST->ConIn->WaitForKey, &EventIndex);
                ST->ConIn->ReadKeyStroke(ST->ConIn, &Key);

                // Clear diagnostics off screen and restore main boot choices
                ST->ConOut->ClearScreen(ST->ConOut);
                Print(L"===================================================================\n");
                Print(L"         RAFA OS (Codenamed ROOSTER) 12.0.1 BOOTLOADER         \n");
                Print(L"===================================================================\n\n");
                Print(L"  [r] Reboot your PC\n");
                Print(L"  [s] Shutdown your PC\n");
                Print(L"  [c] Continue boot\n");
                Print(L"  [u] Boot to UEFI Firmware\n");
                Print(L"  [q] Quit\n");
		Print(L"  [h] Help\n\n");
}
void noefi_help(uint32_t* framebuffer, uint32_t width, uint32_t height, uint32_t pitch)
{
    uint32_t y = 20;

    for (int i = 0; i < help_list_count; i++) {
        draw_string(help_list[i], 20, y, textcolor, framebuffer, pitch);
        y += 20;
    }
    while (1)
  {
    char pressed_key = get_hardware_keypress();
    if (pressed_key != 0)
    {
      break;
    }
  }
}

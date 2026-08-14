#include <efi.h>
#include <efilib.h>
#include "help.h"
#include "drivers.h"

const char *help_list[] = {
    "=========================================",
    "      RAFA OS 10.9.3 HELP       ",
    "=========================================",

    "Rafa OS 1.1.1 is an minmal one file kernel that includes a minmal calclator utillity.",
    "Make sure to vist my github website at: github.com/lucaprograms",
    "Once you get to the main os (kernel) menu, you can press certain keys to control and use the OS (kernel), such as: ",
    "[c]  Open Calclator",
    "[y]  Show version, and show 'Respectfully RAFA OS', plus version",
    "[i]  Open inputbox, where you can preview input",
    "[h]  Open this help menu",
    "[l]  Switch to Amber Mode (Light Mode)",
    "[d]  Switch to Rafa Mode (Dark Mode)",
    "[k]  Print which key is pressed (key debug mode)",
    "[a]  Redraw the main os (kernel) menu",
    "[p]  Power Options",
    "[t]  Show the clock",
    "[o]  Switch to Lily Mode (Teal Mode)",
    "[u]  Switch to Lola Mode (Purple Color Mode)",
    "[g]  Switch to RAFA OS (Green Edition) Mode (Green Color Mode)",
    "[v]  Switch to RAFA OS (Yellow Editition) Mode (Yellow Color Mode)",
    "[b]  Switch to RAFA OS (Red Edition) Mode (Red Color Mode)",
    "[1]  Open number system conversion application",
    "[2]  Set brightness of screen color",
    "[3]  Enable or disable debug mode (key debug mode and allow you to trigger a kernel error.",
    "[4]  Trigger a Kernel Error. Debug mode is needed to be turned on.",
    "[5]  Set password",
    "[6]  Go to login screen",
    "[7]  Play guess random number game",
    "[8]  Launch Diagnostics application",
    "=========================================\r\n",
    "Press any key to return to RAFA OS 1.1.1...\r\n"
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
                Print(L"=========================================\n");
                Print(L"         RAFA OS 10.9.3 BOOTLOADER         \n");
                Print(L"=========================================\n\n");
                Print(L"  [r] Reboot your PC\n");
                Print(L"  [s] Shutdown your PC\n");
                Print(L"  [c] Continue boot\n");
                Print(L"  [u] Boot to UEFI Firmware\n");
                Print(L"  [q] Quit\n");
		Print(L"  [h] Help\n\n");
}
void noefi_help(uint32_t* framebuffer, uint32_t width, uint32_t height, uint32_t pitch)
{
     clear_screen(background, framebuffer, width, height, pitch);
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

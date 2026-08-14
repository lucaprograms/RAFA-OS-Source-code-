// resource.c — Rafa OS Resource Monitor (EFI-kernel)

#include <stdint.h>
#include "drivers.h"   // draw_string, clear_screen, get_hardware_keypress
#include "main.h"
#include <efi.h>
#include <efilib.h>// shutdown(), etc.

// ----------------------------------------------------
// CPUID: get CPU brand string
// ----------------------------------------------------
void get_cpu_brand(char* out) {
    uint32_t a, b, c, d;

    __asm__ volatile("cpuid"
        : "=a"(a), "=b"(b), "=c"(c), "=d"(d)
        : "a"(0x80000002)
    );
    *(uint32_t*)(out + 0)  = a;
    *(uint32_t*)(out + 4)  = b;
    *(uint32_t*)(out + 8)  = c;
    *(uint32_t*)(out + 12) = d;

    __asm__ volatile("cpuid"
        : "=a"(a), "=b"(b), "=c"(c), "=d"(d)
        : "a"(0x80000003)
    );
    *(uint32_t*)(out + 16) = a;
    *(uint32_t*)(out + 20) = b;
    *(uint32_t*)(out + 24) = c;
    *(uint32_t*)(out + 28) = d;

    __asm__ volatile("cpuid"
        : "=a"(a), "=b"(b), "=c"(c), "=d"(d)
        : "a"(0x80000004)
    );
    *(uint32_t*)(out + 32) = a;
    *(uint32_t*)(out + 36) = b;
    *(uint32_t*)(out + 40) = c;
    *(uint32_t*)(out + 44) = d;

    out[48] = 0;
}
static inline void get_hypervisor_signature(UINT32 *ebx, UINT32 *ecx, UINT32 *edx) {
    UINT32 eax = 0x40000000;
    __asm__ __volatile__(
        "cpuid"
        : "=a"(eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(eax)
    );
}

/**
 * Identifies the specific virtualizer running the system.
 * Call this only after confirming the Hypervisor Present Bit is 1.
 * * @return A constant string naming the virtualizer, or "Unknown"
 */
const CHAR8* DetectVirtualizerType(void) {
    UINT32 ebx = 0, ecx = 0, edx = 0;
    CHAR8 signature[13];

    // Query the hypervisor leaf
    get_hypervisor_signature(&ebx, &ecx, &edx);

    // Unpack the 12-byte ASCII string from registers
    *(UINT32 *)(signature + 0) = ebx;
    *(UINT32 *)(signature + 4) = ecx;
    *(UINT32 *)(signature + 8) = edx;
    signature[12] = '\0'; // Null-terminate

    // Match signatures using safe sequential character checks or standard CompareMem
    if (CompareMem(signature, "KVMKVMKVM", 9) == 0) {
        return (const CHAR8*)"QEMU/KVM";
    } 
    if (CompareMem(signature, "TCGTCGTCGTCG", 12) == 0) {
        return (const CHAR8*)"QEMU (TCG Emulator)";
    }   
    if (CompareMem(signature, "VMwareVMware", 12) == 0) {
        return (const CHAR8*)"VMware";
    } 
    if (CompareMem(signature, "Microsoft Hv", 12) == 0) {
        return (const CHAR8*)"Hyper-V";
    } 
    if (CompareMem(signature, "XenVMMXenVMM", 12) == 0) {
        return (const CHAR8*)"Xen";
    } 
    if (CompareMem(signature, "LRMCOF1", 7) == 0) {
        return (const CHAR8*)"VirtualBox";
    } 
    if (CompareMem(signature, "bhyve bhyve ", 12) == 0) {
        return (const CHAR8*)"bhyve";
    }

    // Fallback: If unknown, return the actual string so you can debug it on-screen!
    // Since Rafa OS signature variable is allocated on the stack inside this function, 
    // returning it directly would cause undefined behavior. We use a static buffer instead.
    static CHAR8 unknown_msg[32];
    // Simple manual string copy since standard sprintf isn't available
    CopyMem(unknown_msg, "Unknown: ", 9);
    CopyMem(unknown_msg + 9, signature, 12);
    unknown_msg[21] = '\0';

    return (const CHAR8*)unknown_msg;
}
// ----------------------------------------------------
// Resource monitor app (graphics mode)
// ----------------------------------------------------
int monitorapp(uint32_t* fb, uint32_t pitch) {

    clear_screen(background, fb, 800, 600, pitch);
    draw_string("Rafa OS Device Information Application", 10, 10, textcolor, fb, pitch);

    // CPU brand
    char cpu[64];
    get_cpu_brand(cpu);
    const char* isvm;
    const char* vmtype = DetectVirtualizerType();
    int isvmraw = 0;
    isvmraw = IsVirtualMachine();
    if (isvmraw == 1)
    {
      isvm = "True";
      
    } else 
    {
      isvm = "False";
    }

    draw_string("CPU:", 10, 40, textcolor, fb, pitch);
    draw_string(cpu, 10, 60, textcolor, fb, pitch);
        draw_string("IS A VM:", 10, 85, textcolor, fb, pitch);
    draw_string(isvm, 10, 100, textcolor, fb, pitch);
    if (isvmraw == 1)
    {
      draw_string("Virtualizer type:", 10, 125, textcolor, fb, pitch);
      draw_string(vmtype, 10, 140, textcolor, fb, pitch);
    }

    draw_string("Press any key to exit...", 10, 172, grey/*0xAAAAAA*/, fb, pitch);

    // Input loop (same style as calculator)
    while (1) {
        char c = get_hardware_keypress();
        if (c != 0) break;
    }

    return 0;
}


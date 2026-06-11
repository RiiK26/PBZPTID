#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>
#include <stdint.h>
#include <psapi.h>
#include "Settings.h"
#include "Offsets.h"

static bool FindAntiKickPattern(uintptr_t* outAddress) {
    if (!outAddress) return false;

    uintptr_t moduleBase = (uintptr_t)GetModuleHandleA(NULL);
    if (!moduleBase) return false;

    MODULEINFO modInfo = { 0 };
    if (!GetModuleInformation(GetCurrentProcess(), (HMODULE)moduleBase, &modInfo, sizeof(modInfo))) {
        return false;
    }

    uintptr_t modSize = (uintptr_t)modInfo.SizeOfImage;
    size_t patternLength = sizeof(ArrayOfBytes::AntiKick);
    uintptr_t searchEnd = moduleBase + modSize - patternLength;

    for (uintptr_t addr = moduleBase; addr < searchEnd; addr++) {
        if (memcmp((LPCVOID)addr, ArrayOfBytes::AntiKick, patternLength) == 0) {
            *outAddress = addr;
            return true;
        }
    }
    return false;
}

static void InitAntiKick() {
    if (FindAntiKickPattern(&antiKickAddress)) {
    }
    else {
        antiKickAddress = 0;
    }
}

void ToggleAntiKick(bool enable) {
    if (!antiKickAddress) return;

    DWORD oldProtect;

    // js 0x78 (default), jmp = 0xEB (patch)
    uint8_t patchByte = enable ? 0xEB : 0x78;

    if (VirtualProtect((void*)antiKickAddress, 1, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        *(uint8_t*)antiKickAddress = patchByte;
        VirtualProtect((void*)antiKickAddress, 1, oldProtect, &oldProtect);
    }
}
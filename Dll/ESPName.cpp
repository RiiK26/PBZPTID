#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <windows.h>
#include <psapi.h>
#include "RenderHelper.h"
#include "ESP.h"
#include "Offsets.h"
#include "Settings.h" 
#include "Hooks.h"
#include "InputHelper.h"

constexpr UINT NAMETAG_PATTERN_LENGTH = 11;
constexpr UINT NAMETAG_PATCH_OFFSET = 2;

static DWORD g_NameTagESPAddress = 0;
static BYTE g_NameTagESPOriginalByte = 0x00;
static bool g_NameTagESPFound = false;

static DWORD GetModuleBase() {
    return (DWORD)GetModuleHandleA(NULL);
}

static bool FindNameTagESPPattern(DWORD* outAddress) {
    if (!outAddress) return false;

    DWORD moduleBase = GetModuleBase();
    if (!moduleBase) {
        return false;
    }

    MODULEINFO modInfo = { 0 };
    if (!GetModuleInformation(GetCurrentProcess(), (HMODULE)moduleBase, &modInfo, sizeof(modInfo))) {
        return false;
    }

    DWORD modSize = (DWORD)modInfo.SizeOfImage;
    DWORD searchEnd = moduleBase + modSize - NAMETAG_PATTERN_LENGTH;

    for (DWORD addr = moduleBase; addr < searchEnd; addr++) {
        if (memcmp((LPCVOID)addr, ArrayOfBytes::NameTag, NAMETAG_PATTERN_LENGTH) == 0) {
            *outAddress = addr;
            return true;
        }
    }
    return false;
}

static void ApplyNameTagESPPatch(DWORD address, bool enable) {
    if (!address) {
        return;
    }

    DWORD oldProtect = 0;
    BYTE patchByte = enable ? 0x01 : 0x00;
    BYTE* patchAddr = (BYTE*)(address + NAMETAG_PATCH_OFFSET);

    if (!VirtualProtect(patchAddr, 1, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        return;
    }

    BYTE currentByte = *patchAddr;
    *patchAddr = patchByte;
    VirtualProtect(patchAddr, 1, oldProtect, &oldProtect);
}

static void InitializeNameTagESP() {
    if (FindNameTagESPPattern(&g_NameTagESPAddress)) {
        g_NameTagESPFound = true;
        g_NameTagESPOriginalByte = *(BYTE*)(g_NameTagESPAddress + NAMETAG_PATCH_OFFSET);
    }
    else {
        g_NameTagESPFound = false;
    }
}

void ToggleNameTagESP(bool enable) {
    if (!g_NameTagESPFound) {
        if (!FindNameTagESPPattern(&g_NameTagESPAddress)) {
            return;
        }
        g_NameTagESPFound = true;
    }

    ApplyNameTagESPPatch(g_NameTagESPAddress, enable);
}

static void ShutdownNameTagESP() {
    if (g_NameTagESPFound && g_NameTagESPAddress) {
        ApplyNameTagESPPatch(g_NameTagESPAddress, false);
    }
}
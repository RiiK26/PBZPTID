#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>
#include <iostream>
#include <psapi.h>
#include "Hooks.h"
#include "Macro.h"
#include "Settings.h"
#include "ESP.h"
#include "XOR.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib, "Psapi.lib")

HANDLE hMacroThread = NULL;

static DWORD WINAPI MainThread(LPVOID lpParam) {
    HMODULE hModule = (HMODULE)lpParam;

    dwPBBase = (DWORD)GetModuleHandleA(NULL);
    while (GetModuleHandleA(XOR("d3d9.dll")) == NULL) Sleep(100);

    hMacroThread = CreateThread(NULL, 0, MacroThread, NULL, 0, NULL);

    InitializeHooks();
    while (bIsRunning) {
        if (GetAsyncKeyState(VK_END) & 1) {
            EnterCriticalSection(&csRunning);
            bIsRunning = false;
            LeaveCriticalSection(&csRunning);
        }
        Sleep(50);
    }
    Sleep(200);

    UninitializeHooks();

    if (hMacroThread) {
        WaitForSingleObject(hMacroThread, INFINITE);
        CloseHandle(hMacroThread);
        hMacroThread = NULL;
    }

    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        InitializeCriticalSection(&csRunning);
        CreateThread(NULL, 0, MainThread, hModule, 0, NULL);
    }
    return TRUE;
}
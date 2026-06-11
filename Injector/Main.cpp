#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <string>
#include "XOR.h"

enum ConsoleColors : WORD {
    RED = 12,
    GREEN = 10,
    YELLOW = 14,
    WHITE = 15,
    CYAN = 11
};

static void SetColor(WORD color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

static void EnableDebugPriv() {
    HANDLE hToken;
    LUID luid = { 0 };

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        LookupPrivilegeValueA(NULL, XOR("SeDebugPrivilege"), &luid);
        TOKEN_PRIVILEGES tp = { 0 };

        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
        CloseHandle(hToken);
    }
}

static DWORD GetProcId(const char* procName) {
    DWORD procId = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 procEntry = { 0 };
        procEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnap, &procEntry)) {
            do {
                if (!_stricmp(procEntry.szExeFile, procName)) {
                    procId = procEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(hSnap, &procEntry));
        }
    }
    CloseHandle(hSnap);
    return procId;
}

int main() {
    system("mode con cols=28 lines=3");

    HWND consoleWindow = GetConsoleWindow();
    if (consoleWindow != NULL) {
        int PosX = 50;
        int PosY = 50;

        SetWindowPos(consoleWindow, 0, PosX, PosY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }

    SetConsoleTitleA("PB Simple");
    EnableDebugPriv();

    char dllPath[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, dllPath, MAX_PATH);

    char* lastBackslash = strrchr(dllPath, '\\');
    if (lastBackslash) {
        strcpy_s(lastBackslash + 1, MAX_PATH - (lastBackslash + 1 - dllPath), "Dll.dll");
    }

    const char* targetProcess = XOR("PointBlank.exe");

    SetColor(CYAN);
    std::cout << ">> Start Point Blank now!!" << std::endl;

    DWORD procId = 0;
    while (!procId) {
        procId = GetProcId(targetProcess);
        Sleep(50);
    }

    Sleep(1500);

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, procId);
    if (hProc && hProc != INVALID_HANDLE_VALUE) {
        void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        if (loc) {
            WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);
            HMODULE hKernel32 = GetModuleHandleA(XOR("Kernel32.dll"));
            if (hKernel32 != NULL) {
                FARPROC loadLibAddr = GetProcAddress(hKernel32, XOR("LoadLibraryA"));

                if (loadLibAddr != NULL) {
                    HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)loadLibAddr, loc, 0, 0);

                    if (hThread) {
                        WaitForSingleObject(hThread, INFINITE);
                        DWORD hLibModule = 0;
                        GetExitCodeThread(hThread, &hLibModule);

                        if (hLibModule != 0) {
                            SetColor(GREEN);
                            std::cout << "[+] " << targetProcess << " Injection Success!" << std::endl;
                        }
                        else {
                            SetColor(RED);
                            std::cout << "\n[X] FATAL ERROR: DLL ERROR!" << std::endl;
                            std::cout << "[X] Cause: Wrong Path or Wrong Architecture." << std::endl;
                        }
                        CloseHandle(hThread);
                    }
                    else {
                        SetColor(RED);
                        std::cout << "\n[X] FAIL: Thread cancelled by Anti-Cheat" << std::endl;
                    }
                }
            }
            VirtualFreeEx(hProc, loc, 0, MEM_RELEASE);
        }
        CloseHandle(hProc);
    }
    return 0;
}
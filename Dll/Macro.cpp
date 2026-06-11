#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>
#include "Macro.h"
#include "Settings.h"
#include "MacroUtils.h"

static void SendKey(BYTE key) {
	keybd_event(key, 0, 0, 0);
	Sleep(20);
	keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
}

static void SendMouseClick(DWORD dwDown, DWORD dwUp) {
	mouse_event(dwDown, 0, 0, 0, 0);
	Sleep(20);
	mouse_event(dwUp, 0, 0, 0, 0);
}

DWORD WINAPI MacroThread(LPVOID lpParam) {
	bool isLeftHeld = false;
	bool isRightHeld = false;

	while (true) {
		Sleep(1);

		EnterCriticalSection(&csRunning);
		bool shouldRun = bIsRunning;
		LeaveCriticalSection(&csRunning);

		if (!shouldRun) break;

		if (macroEnabled) {
			HWND fw = GetForegroundWindow();
			DWORD procId; GetWindowThreadProcessId(fw, &procId);
			if (procId != GetCurrentProcessId()) continue;

			if (macroMode == 1) {
				if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
					if (!isRightHeld) {
						isRightHeld = true;
						Sleep(GetActiveDelay());
						SendMouseClick(MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP);
						Sleep(GetActiveDelay());
						SendKey(0x33);
						Sleep(GetActiveDelay());
						SendKey(0x51);
					}
				}
				else { isRightHeld = false; }
			}
			else if (macroMode == 2) {
				if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
					if (!isLeftHeld) {
						isLeftHeld = true;
						Sleep(GetActiveDelay());
						SendKey(0x33);
						Sleep(GetActiveDelay());
						SendKey(0x51);
					}
				}
				else { isLeftHeld = false; }
			}
		}
	}
	return 0;
}
#pragma once

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>
#include "Hooks.h"

extern bool	showMenu;
extern int	menuIndex;
extern bool	catVisualOpen;
extern bool	catWeaponOpen;
extern bool	catPlayerOpen;
extern bool catSettingsOpen;

extern int	crosshairMode;

extern int	ESPBody;
extern bool	ESPName;

extern bool antiKickEnabled;
extern uintptr_t antiKickAddress;

extern bool	isTypingDelay;
extern bool	grabMatrixThisFrame;

extern bool macroEnabled;
extern int macroMode;
extern int delayPreset;
extern int customDelayValue;

extern bool	bIsRunning;
extern DWORD dwPBBase;
extern CRITICAL_SECTION csRunning;

extern int currentTheme;
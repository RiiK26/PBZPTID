#include "Settings.h"

DWORD dwPBBase = 0;
bool bIsRunning = true;
CRITICAL_SECTION csRunning;

bool showMenu = true;
int menuIndex = 0;
bool catVisualOpen = false;
bool catWeaponOpen = false;
bool catPlayerOpen = false;
bool catSettingsOpen = false;

int crosshairMode = 0;
int ESPBody = 0;
bool ESPName = false;

bool antiKickEnabled = false;
uintptr_t antiKickAddress = 0;

bool isTypingDelay = false;
bool grabMatrixThisFrame = false;

bool macroEnabled = false;
int macroMode = 1;
int delayPreset = 1;
int customDelayValue = 120;

int currentTheme = 0; // 0: Mint, 1: Cyan, 2: Crimson, 3: Purple, 4: Gold
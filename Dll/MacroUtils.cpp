#include "MacroUtils.h"
#include "Settings.h"

int GetActiveDelay() {
    if (delayPreset == 0) return 25;
    if (delayPreset == 1) return 50;
    if (delayPreset == 2) return 75;
    if (delayPreset == 3) return 100;
    return customDelayValue;
}
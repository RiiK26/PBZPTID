#include "InputHelper.h"

bool KeyPressed(int vKey) {
    static bool pressed[256] = { false };
    if (GetAsyncKeyState(vKey) & 0x8000) {
        if (!pressed[vKey]) {
            pressed[vKey] = true;
            return true;
        }
    }
    else {
        pressed[vKey] = false;
    }
    return false;
}
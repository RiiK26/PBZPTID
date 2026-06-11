#include <stdio.h>
#include <d3d9.h>
#include "RenderHelper.h"
#include "Settings.h"
#include "Menu.h"
#include "InputHelper.h"
#include "ESP.h"
#include "Players.h"
#include "XOR.h"

static D3DCOLOR GetThemeColor() {
    switch (currentTheme) {
    case 0: return D3DCOLOR_ARGB(255, 0, 220, 120);
    case 1: return D3DCOLOR_ARGB(255, 0, 180, 255);
    case 2: return D3DCOLOR_ARGB(255, 230, 40, 60);
    case 3: return D3DCOLOR_ARGB(255, 160, 50, 255);
    case 4: return D3DCOLOR_ARGB(255, 255, 190, 0);
    default: return D3DCOLOR_ARGB(255, 255, 255, 255);
    }
}

void DrawMenu(LPDIRECT3DDEVICE9 pDevice) {
    if (!showMenu) return;

    DWORD oldZEnable, oldScissor, oldAlphaBlend, oldSrcBlend, oldDestBlend;
    pDevice->GetRenderState(D3DRS_ZENABLE, &oldZEnable);
    pDevice->GetRenderState(D3DRS_SCISSORTESTENABLE, &oldScissor);
    pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlphaBlend);
    pDevice->GetRenderState(D3DRS_SRCBLEND, &oldSrcBlend);
    pDevice->GetRenderState(D3DRS_DESTBLEND, &oldDestBlend);

    pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    D3DVIEWPORT9 vp;
    pDevice->GetViewport(&vp);

    int guiX = (int)(vp.Width * 0.02f);
    int guiY = (int)(vp.Height * 0.02f);
    if (guiX < 20) guiX = 20;
    if (guiY < 10) guiY = 10;

    int guiW = 175;
    int borderThick = 2;
    int spacing = 18;

    int totalItems = 4;
    if (catVisualOpen) totalItems += 5;
    if (catWeaponOpen) {
        totalItems += 1;
        if (macroEnabled) {
            totalItems += 2;
            if (delayPreset == 4) totalItems += 1;
        }
    }
    if (catPlayerOpen) totalItems += 1;
    if (catSettingsOpen) totalItems += 1;

    int guiH = 32 + (totalItems * spacing) + 10;

    D3DCOLOR themeColor = GetThemeColor();
    D3DCOLOR bgColor = D3DCOLOR_ARGB(210, 18, 18, 20);
    D3DCOLOR textColor = D3DCOLOR_ARGB(255, 235, 235, 235);
    D3DCOLOR inactiveCatColor = D3DCOLOR_ARGB(255, 150, 150, 150);

    // Background
    FillRGB(guiX, guiY, guiW, guiH, bgColor, pDevice);

    // Borders
    FillRGB(guiX - borderThick, guiY - borderThick, guiW + (borderThick * 2), borderThick, themeColor, pDevice); // Top
    FillRGB(guiX - borderThick, guiY + guiH, guiW + (borderThick * 2), borderThick, themeColor, pDevice); // Bottom
    FillRGB(guiX - borderThick, guiY, borderThick, guiH, themeColor, pDevice); // Left
    FillRGB(guiX + guiW, guiY, borderThick, guiH, themeColor, pDevice); // Right

    // Draw Header
    FillRGB(guiX, guiY, guiW, 25, themeColor, pDevice);
    DrawTextStr(guiX + 10, guiY + 6, guiW - 20, D3DCOLOR_ARGB(255, 20, 20, 20), XOR("RiiK Hub"));

    int currentY = guiY + 32;
    int currentIndex = 0;
    char buffer[64];

#define GET_COLOR(idx) (menuIndex == (idx) ? themeColor : textColor)
#define GET_CAT_COLOR(idx) (menuIndex == (idx) ? themeColor : inactiveCatColor)

    sprintf_s(buffer, XOR("%s VISUAL"), catVisualOpen ? XOR("-") : XOR("+"));
    DrawTextStr(guiX + 10, currentY, guiW - 20, GET_CAT_COLOR(currentIndex), buffer);
    currentIndex++; currentY += spacing;

    if (catVisualOpen) {
        sprintf_s(buffer, XOR("Crosshair    [ %s ]"), crosshairMode == 0 ? XOR("OFF") : (crosshairMode == 1 ? XOR("DOT") : XOR("CROSS")));
        DrawTextStr(guiX + 20, currentY, guiW - 30, GET_COLOR(currentIndex), buffer);
        currentIndex++; currentY += spacing;

        sprintf_s(buffer, XOR("ESP Body     [ %s ]"), ESPBody ? XOR("ON") : XOR("OFF"));
        DrawTextStr(guiX + 20, currentY, guiW - 30, GET_COLOR(currentIndex), buffer);
        currentIndex++; currentY += spacing;

        sprintf_s(buffer, XOR("ESP Name     [ %s ]"), ESPName ? XOR("ON") : XOR("OFF"));
        DrawTextStr(guiX + 20, currentY, guiW - 30, GET_COLOR(currentIndex), buffer);
        currentIndex++; currentY += spacing;
    }

    sprintf_s(buffer, XOR("%s WEAPON"), catWeaponOpen ? "-" : "+");
    DrawTextStr(guiX + 10, currentY, guiW - 20, GET_CAT_COLOR(currentIndex), buffer);
    currentIndex++; currentY += spacing;

    if (catWeaponOpen) {
        sprintf_s(buffer, XOR("Macro Toggle [ %s ]"), macroEnabled ? XOR("ON") : XOR("OFF"));
        DrawTextStr(guiX + 20, currentY, guiW - 30, GET_COLOR(currentIndex), buffer);
        currentIndex++; currentY += spacing;

        if (macroEnabled) {
            sprintf_s(buffer, XOR("Macro Type   [ %s ]"), (macroMode == 1) ? XOR("Scope 3Q") : XOR("3Q"));
            DrawTextStr(guiX + 20, currentY, guiW - 30, GET_COLOR(currentIndex), buffer);
            currentIndex++; currentY += spacing;

            const char* presetStr = (delayPreset == 0) ? XOR("25ms") : ((delayPreset == 1) ? XOR("50ms") : ((delayPreset == 2) ? XOR("75ms") : ((delayPreset == 3) ? XOR("100ms") : XOR("CUSTOM"))));
            sprintf_s(buffer, XOR("Delay Preset [ %s ]"), presetStr);
            DrawTextStr(guiX + 20, currentY, guiW - 30, GET_COLOR(currentIndex), buffer);
            currentIndex++; currentY += spacing;

            if (delayPreset == 4) {
                if (isTypingDelay) sprintf_s(buffer, XOR(" > Value: %d_"), customDelayValue);
                else sprintf_s(buffer, XOR(" > Value: [ %d ]"), customDelayValue);

                D3DCOLOR customColor = (isTypingDelay) ? D3DCOLOR_ARGB(255, 255, 100, 100) : GET_COLOR(currentIndex);
                DrawTextStr(guiX + 30, currentY, guiW - 40, customColor, buffer);
                currentIndex++; currentY += spacing;
            }
        }
    }

    sprintf_s(buffer, XOR("%s PLAYER"), catPlayerOpen ? "-" : "+");
    DrawTextStr(guiX + 10, currentY, guiW - 20, GET_CAT_COLOR(currentIndex), buffer);
    currentIndex++; currentY += spacing;

    if (catPlayerOpen) {
        sprintf_s(buffer, XOR("Anti Kick    [ %s ]"), antiKickEnabled ? "ON" : "OFF");
        DrawTextStr(guiX + 20, currentY, guiW - 30, GET_COLOR(currentIndex), buffer);
        currentIndex++; currentY += spacing;
    }

    sprintf_s(buffer, XOR("%s SETTINGS"), catSettingsOpen ? "-" : "+");
    DrawTextStr(guiX + 10, currentY, guiW - 20, GET_CAT_COLOR(currentIndex), buffer);
    currentIndex++; currentY += spacing;

    if (catSettingsOpen) {
        const char* themeStr = (currentTheme == 0) ? XOR("MINT") :
            (currentTheme == 1) ? XOR("CYAN") :
            (currentTheme == 2) ? XOR("CRIMSON") :
            (currentTheme == 3) ? XOR("PURPLE") : XOR("GOLD");
        sprintf_s(buffer, XOR("UI Theme     [ %s ]"), themeStr);
        DrawTextStr(guiX + 20, currentY, guiW - 30, GET_COLOR(currentIndex), buffer);
        currentIndex++; currentY += spacing;
    }

    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, oldAlphaBlend);
    pDevice->SetRenderState(D3DRS_SRCBLEND, oldSrcBlend);
    pDevice->SetRenderState(D3DRS_DESTBLEND, oldDestBlend);
    pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, oldScissor);
    pDevice->SetRenderState(D3DRS_ZENABLE, oldZEnable);
}

enum MenuItems {
    ITEM_CAT_VISUAL, ITEM_CROSSHAIR, ESP_BODY, ESP_NAME,
    ITEM_CAT_WEAPON, ITEM_MACROTOGGLE, ITEM_MACROMODE, ITEM_DELAYPRESET, ITEM_DELAYCUSTOM,
    ITEM_CAT_PLAYER, ITEM_ANTIKICK,
    ITEM_CAT_SETTINGS, ITEM_THEME
};

void UpdateMenuInput() {
    if (KeyPressed(VK_INSERT)) showMenu = !showMenu;

    if (showMenu) {
        if (isTypingDelay) {
            for (int i = 0; i <= 9; i++) {
                if (KeyPressed('0' + i) || KeyPressed(VK_NUMPAD0 + i)) {
                    if (customDelayValue < 999) {
                        customDelayValue = (customDelayValue * 10) + i;
                    }
                }
            }
            if (KeyPressed(VK_BACK)) customDelayValue /= 10;
            if (KeyPressed(VK_RETURN) || KeyPressed(VK_ESCAPE)) isTypingDelay = false;
        }
        else {
            int visibleItems[30] = { 0 };
            int numItems = 0;

            visibleItems[numItems++] = ITEM_CAT_VISUAL;
            if (catVisualOpen) {
                visibleItems[numItems++] = ITEM_CROSSHAIR;
                visibleItems[numItems++] = ESP_BODY;
                visibleItems[numItems++] = ESP_NAME;
            }

            visibleItems[numItems++] = ITEM_CAT_WEAPON;
            if (catWeaponOpen) {
                visibleItems[numItems++] = ITEM_MACROTOGGLE;
                if (macroEnabled) {
                    visibleItems[numItems++] = ITEM_MACROMODE;
                    visibleItems[numItems++] = ITEM_DELAYPRESET;
                    if (delayPreset == 4) visibleItems[numItems++] = ITEM_DELAYCUSTOM;
                }
            }

            visibleItems[numItems++] = ITEM_CAT_PLAYER;
            if (catPlayerOpen) {
                visibleItems[numItems++] = ITEM_ANTIKICK;
            }

            visibleItems[numItems++] = ITEM_CAT_SETTINGS;
            if (catSettingsOpen) {
                visibleItems[numItems++] = ITEM_THEME;
            }

            int maxIndex = numItems - 1;
            if (menuIndex > maxIndex) menuIndex = maxIndex;

            bool keyUp = KeyPressed(VK_UP);
            bool keyDown = KeyPressed(VK_DOWN);
            bool keyRight = KeyPressed(VK_RIGHT);
            bool keyLeft = KeyPressed(VK_LEFT);
            bool keyEnter = KeyPressed(VK_RETURN);

            if (keyUp)   if (menuIndex > 0) menuIndex--;
            if (keyDown) if (menuIndex < maxIndex) menuIndex++;

            int activeItem = visibleItems[menuIndex];

            if (keyRight || keyEnter) {
                switch (activeItem) {
                case ITEM_CAT_VISUAL:   if (keyRight) catVisualOpen = true; break;
                case ITEM_CAT_WEAPON:   if (keyRight) catWeaponOpen = true; break;
                case ITEM_CAT_PLAYER:   if (keyRight) catPlayerOpen = true; break;
                case ITEM_CAT_SETTINGS: if (keyRight) catSettingsOpen = true; break;

                case ITEM_CROSSHAIR: if (keyRight && crosshairMode < 2) crosshairMode++; break;
                case ESP_BODY:       if (keyRight && ESPBody < 1) ESPBody++; break;
                case ESP_NAME:
                    if (keyEnter) { ESPName = !ESPName; ToggleNameTagESP(ESPName); }
                    else if (keyRight) { ESPName = true; ToggleNameTagESP(ESPName); }
                    break;

                case ITEM_MACROTOGGLE:
                    if (keyEnter) macroEnabled = !macroEnabled;
                    else if (keyRight) macroEnabled = true;
                    break;

                case ITEM_MACROMODE:   if (keyRight && macroMode < 2) macroMode++; break;
                case ITEM_DELAYPRESET: if (keyRight && delayPreset < 4) delayPreset++; break;

                case ITEM_DELAYCUSTOM:
                    if (keyEnter || keyRight) isTypingDelay = true;
                    break;

                case ITEM_ANTIKICK:
                    if (keyEnter) {
                        antiKickEnabled = !antiKickEnabled;
                        ToggleAntiKick(antiKickEnabled);
                    }
                    else if (keyRight) {
                        antiKickEnabled = true;
                        ToggleAntiKick(true);
                    }
                    break;

                case ITEM_THEME:
                    if (keyRight && currentTheme < 4) currentTheme++;
                    break;
                }
            }

            if (keyLeft) {
                switch (activeItem) {
                case ITEM_CAT_VISUAL:   catVisualOpen = false; break;
                case ITEM_CAT_WEAPON:   catWeaponOpen = false; break;
                case ITEM_CAT_PLAYER:   catPlayerOpen = false; break;
                case ITEM_CAT_SETTINGS: catSettingsOpen = false; break;

                case ITEM_CROSSHAIR:    if (crosshairMode > 0) crosshairMode--; break;
                case ESP_BODY:          if (ESPBody > 0) ESPBody--; break;
                case ESP_NAME:
                    ESPName = false;
                    ToggleNameTagESP(false);
                    break;

                case ITEM_MACROTOGGLE:  macroEnabled = false; break;
                case ITEM_MACROMODE:    if (macroMode > 1) macroMode--; break;
                case ITEM_DELAYPRESET:  if (delayPreset > 0) delayPreset--; break;

                case ITEM_ANTIKICK:
                    if (antiKickEnabled) {
                        antiKickEnabled = false;
                        ToggleAntiKick(false);
                    }
                    break;

                case ITEM_THEME:        if (currentTheme > 0) currentTheme--; break;
                }
            }
        }
    }
}
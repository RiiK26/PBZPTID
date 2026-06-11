#pragma once
#include <d3d9.h>

void DrawCrosshair(LPDIRECT3DDEVICE9 pDevice);

void InitializeNameTagESP();
void ToggleNameTagESP(bool enable);
void ShutdownNameTagESP();
bool FindNameTagESPPattern(DWORD* outAddress);
void ApplyNameTagESPPatch(DWORD address, bool enable);
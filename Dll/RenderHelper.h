#pragma once
#include <d3d9.h>
#include <d3dx9.h>

extern ID3DXFont* pFont;
void FillRGB(int x, int y, int w, int h, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice);
void DrawTextStr(int x, int y, int w, D3DCOLOR color, const char* text);
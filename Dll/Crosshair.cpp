#include "ESP.h"
#include "RenderHelper.h"
#include "Settings.h"
#include <d3d9.h>

void DrawCrosshair(LPDIRECT3DDEVICE9 pDevice) {
    D3DVIEWPORT9 vp;
    pDevice->GetViewport(&vp);

    int centerX = vp.Width / 2;
    int centerY = vp.Height / 2;

    if (crosshairMode == 1)
        FillRGB(centerX - 2, centerY - 2, 4, 4, D3DCOLOR_ARGB(255, 255, 0, 0), pDevice);
    else if (crosshairMode == 2) {
        FillRGB(centerX - 10, centerY - 1, 20, 2, D3DCOLOR_ARGB(255, 0, 255, 0), pDevice);
        FillRGB(centerX - 1, centerY - 10, 2, 20, D3DCOLOR_ARGB(255, 0, 255, 0), pDevice);
    }
}
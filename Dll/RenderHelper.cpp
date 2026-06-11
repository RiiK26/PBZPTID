#include "RenderHelper.h"

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)
ID3DXFont* pFont = NULL;

struct D3DVERTEX {
    float x, y, z, rhw;
    D3DCOLOR color;
};

void FillRGB(int x, int y, int w, int h, D3DCOLOR color, LPDIRECT3DDEVICE9 pDevice) {
    D3DVERTEX vertices[4] = {
        { (float)x, (float)(y + h), 0.0f, 1.0f, color },
        { (float)x, (float)y, 0.0f, 1.0f, color },
        { (float)(x + w), (float)(y + h), 0.0f, 1.0f, color },
        { (float)(x + w), (float)y, 0.0f, 1.0f, color }
    };

    DWORD oldZEnable, oldAlphaBlend, oldScissor, oldColorOp, oldColorArg, oldCullMode, oldSrcBlend, oldDestBlend;
    IDirect3DPixelShader9* pShader = NULL;
    IDirect3DVertexShader9* pVertexShader = NULL;
    DWORD oldColorWrite;

    pDevice->GetVertexShader(&pVertexShader);
    pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &oldColorWrite);

    pDevice->SetVertexShader(NULL);
    pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);

    pDevice->GetPixelShader(&pShader);
    pDevice->GetRenderState(D3DRS_ZENABLE, &oldZEnable);
    pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &oldAlphaBlend);
    pDevice->GetRenderState(D3DRS_SCISSORTESTENABLE, &oldScissor);
    pDevice->GetRenderState(D3DRS_CULLMODE, &oldCullMode);
    pDevice->GetRenderState(D3DRS_SRCBLEND, &oldSrcBlend);
    pDevice->GetRenderState(D3DRS_DESTBLEND, &oldDestBlend);
    pDevice->GetTextureStageState(0, D3DTSS_COLOROP, &oldColorOp);
    pDevice->GetTextureStageState(0, D3DTSS_COLORARG1, &oldColorArg);

    pDevice->SetPixelShader(NULL);
    pDevice->SetTexture(0, NULL);
    pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
    pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

    pDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
    pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(D3DVERTEX));

    pDevice->SetTextureStageState(0, D3DTSS_COLOROP, oldColorOp);
    pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, oldColorArg);
    pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, oldScissor);
    pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, oldAlphaBlend);
    pDevice->SetRenderState(D3DRS_ZENABLE, oldZEnable);
    pDevice->SetRenderState(D3DRS_CULLMODE, oldCullMode);
    pDevice->SetRenderState(D3DRS_SRCBLEND, oldSrcBlend);
    pDevice->SetRenderState(D3DRS_DESTBLEND, oldDestBlend);

    if (pShader != NULL) {
        pDevice->SetPixelShader(pShader);
        pShader->Release();
    }

    pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, oldColorWrite);
    if (pVertexShader != NULL) {
        pDevice->SetVertexShader(pVertexShader);
        pVertexShader->Release();
    }
}

void DrawTextStr(int x, int y, int w, D3DCOLOR color, const char* text) {
    if (!pFont || !text) return;

    RECT rect = { x, y, x + w, y + 25 };
    RECT shadowRect = { x + 1, y + 1, x + w + 1, y + 26 };

    HRESULT hrShadow = pFont->DrawTextA(NULL, text, -1, &shadowRect, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB(255, 0, 0, 0));
    if (FAILED(hrShadow)) return;

    HRESULT hrMain = pFont->DrawTextA(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, color);
    if (FAILED(hrMain)) {
        return;
    }
}
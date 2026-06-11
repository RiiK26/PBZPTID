#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include "Hooks.h"
#include "WH.h"
#include "ESP.h"
#include "RenderHelper.h"
#include "Settings.h"
#include "Menu.h"
#include <windows.h>
#include <cstdio>
#include "libs/MinHook.h"

tEndScene oEndScene = NULL;
tDrawIndexedPrimitive oDrawIndexedPrimitive = NULL;
tReset oReset = NULL;

static IDirect3DDevice9* lastDevice = NULL;

HRESULT APIENTRY hkReset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
    if (pFont != NULL) {
        pFont->OnLostDevice();
    }

    HRESULT hr = oReset(pDevice, pPresentationParameters);
    if (SUCCEEDED(hr)) {
        if (pFont != NULL) {
            pFont->OnResetDevice();
        }
    }
    return hr;
}

HRESULT APIENTRY hkEndScene(LPDIRECT3DDEVICE9 pDevice) {
    if (!bIsRunning) {
        if (pFont != NULL) {
            pFont->Release();
            pFont = NULL;
        }
        lastDevice = NULL;
        return oEndScene(pDevice);
    }

    if (pDevice != lastDevice) {
        if (pFont != NULL) {
            pFont->Release();
            pFont = NULL;
        }
        lastDevice = pDevice;
    }

    if (pFont == NULL) {
        D3DXCreateFont(pDevice, 14, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont);
    }

    if (pFont != NULL) {
        UpdateMenuInput();
        DrawMenu(pDevice);
    }

    DrawCrosshair(pDevice);

    return oEndScene(pDevice);
}

HRESULT APIENTRY hkDrawIndexedPrimitive(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) {
    return DoWallhack(pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}

static bool GetD3D9VTable(void** vTableArray) {
    IDirect3D9* pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!pD3D) return false;

    D3DPRESENT_PARAMETERS d3dpp = { 0 };
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

    d3dpp.hDeviceWindow = GetDesktopWindow();
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    d3dpp.BackBufferCount = 1;

    IDirect3DDevice9* pDevice = NULL;

    HRESULT hr = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDevice);

    if (FAILED(hr) || !pDevice) {
        pD3D->Release();
        return false;
    }

    void** vTable = *(void***)pDevice;
    vTableArray[0] = vTable[16]; // Reset
	vTableArray[1] = vTable[42]; // EndScene
	vTableArray[2] = vTable[82]; // DrawIndexedPrimitive

    pDevice->Release();
    pD3D->Release();

    return true;
}

bool InitializeHooks() {
    void* d3d9Functions[3];

    if (GetD3D9VTable(d3d9Functions)) {
        if (MH_Initialize() == MH_OK) {
            if (MH_CreateHook(d3d9Functions[0], &hkReset, (LPVOID*)&oReset) != MH_OK) return false;
            if (MH_CreateHook(d3d9Functions[1], &hkEndScene, (LPVOID*)&oEndScene) != MH_OK) return false;
            if (MH_CreateHook(d3d9Functions[2], &hkDrawIndexedPrimitive, (LPVOID*)&oDrawIndexedPrimitive) != MH_OK) return false;
            if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) return false;
        }
    }
    return false;
}

void UninitializeHooks() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    if (pFont) {
        pFont->Release();
        pFont = nullptr;
    }
}
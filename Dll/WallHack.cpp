#include "WH.h"
#include "Settings.h"
#include "Hooks.h"
//#include <iostream> uncomment this for debug
#include <d3d9.h>

constexpr UINT PLAYER_BODY_1                = 36;
constexpr UINT PLAYER_BODY_2                = 44;
constexpr UINT PLAYER_BODY_3                = 48;
constexpr UINT PLAYER_BODY_4                = 52;
constexpr UINT STRIDE_PLAYER_PART           = 32;

constexpr UINT MIN_PLAYER_BODY_VERTICES     = 50;
constexpr UINT MIN_PLAYER_PART_VERTICES     = 500;
constexpr UINT MAX_PLAYER_PART_VERTICES     = 3000;
constexpr UINT MIN_MAP_UI_VERTICES          = 150;
constexpr UINT UI_VERTEX_COUNT              = 4096;
constexpr UINT STRIDE_MAP_UI                = 40;

constexpr UINT EXCLUDED_VERTEX_COUNT_1      = 1014;
constexpr UINT EXCLUDED_VERTEX_COUNT_2      = 510;
constexpr UINT EXCLUDED_VERTEX_COUNT_3      = 588;
constexpr UINT EXCLUDED_VERTEX_COUNT_4      = 536;
constexpr UINT EXCLUDED_VERTEX_COUNT_5      = 1067;
constexpr UINT EXCLUDED_VERTEX_COUNT_6      = 1270;
constexpr UINT EXCLUDED_VERTEX_COUNT_7      = 1292;
constexpr UINT EXCLUDED_VERTEX_COUNT_8      = 2276;
constexpr UINT EXCLUDED_VERTEX_COUNT_9      = 1009;
constexpr UINT EXCLUDED_VERTEX_COUNT_10     = 932;
constexpr UINT EXCLUDED_VERTEX_COUNT_11     = 624;

HRESULT DoWallhack(LPDIRECT3DDEVICE9 pDevice, D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) {

    IDirect3DVertexBuffer9* pStreamData = NULL;
    UINT iOffsetInBytes = 0, iStride = 0;

    if (pDevice->GetStreamSource(0, &pStreamData, &iOffsetInBytes, &iStride) != D3D_OK) {
        return oDrawIndexedPrimitive(pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
    }

    if (pStreamData) {
        pStreamData->Release();
    }

    bool isPlayerBody = false;
    bool isPlayerPart = false;

    if (NumVertices > MIN_PLAYER_BODY_VERTICES) {
        switch (iStride) {
        case PLAYER_BODY_1:
        case PLAYER_BODY_2:
        case PLAYER_BODY_3:
        case PLAYER_BODY_4:
            isPlayerBody = true;
            break;
        }
    }

    if (iStride == STRIDE_PLAYER_PART && NumVertices > MIN_PLAYER_PART_VERTICES && NumVertices < MAX_PLAYER_PART_VERTICES) {
        isPlayerPart = true;

        if ((NumVertices >= 500 && NumVertices <= 8000)) {
            isPlayerPart = false;
        }
        else {
            switch (NumVertices) {
            case EXCLUDED_VERTEX_COUNT_1:
            case EXCLUDED_VERTEX_COUNT_2:
            case EXCLUDED_VERTEX_COUNT_3:
            case EXCLUDED_VERTEX_COUNT_4:
            case EXCLUDED_VERTEX_COUNT_5:
            case EXCLUDED_VERTEX_COUNT_6:
            case EXCLUDED_VERTEX_COUNT_7:
            case EXCLUDED_VERTEX_COUNT_8:
            case EXCLUDED_VERTEX_COUNT_9:
            case EXCLUDED_VERTEX_COUNT_10:
            case EXCLUDED_VERTEX_COUNT_11:
                isPlayerPart = false;
                break;
            }
        }
    }

    if (iStride == STRIDE_MAP_UI || NumVertices == UI_VERTEX_COUNT || NumVertices < MIN_MAP_UI_VERTICES) {
        isPlayerBody = false;
        isPlayerPart = false;
    }

    if ((isPlayerBody || isPlayerPart) && ESPBody > 0) {
        DWORD dwOldZEnable, dwOldZFunc;
        pDevice->GetRenderState(D3DRS_ZENABLE, &dwOldZEnable);
        pDevice->GetRenderState(D3DRS_ZFUNC, &dwOldZFunc);

        pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
        pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);

        HRESULT ret = oDrawIndexedPrimitive(pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);

        pDevice->SetRenderState(D3DRS_ZENABLE, dwOldZEnable);
        pDevice->SetRenderState(D3DRS_ZFUNC, dwOldZFunc);

        return ret;
    }

    /* // For Debugging
    if (GetAsyncKeyState(VK_F8) & 0x8000) {
        if (iStride != STRIDE_MAP_UI && NumVertices != UI_VERTEX_COUNT && NumVertices > MIN_MAP_UI_VERTICES) {
            if (!isPlayerBody && !isPlayerPart) {
                printf("Character Found!!! -> NumVert: %d | STRIDE: %d\n", NumVertices, iStride);
            }
        }
    }

        if (GetAsyncKeyState(VK_F7) & 0x8000 && isPlayerPart) {
        printf("Anomali Found -> NumVert: %d\n", NumVertices);
    }
    */

    return oDrawIndexedPrimitive(pDevice, Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount);
}
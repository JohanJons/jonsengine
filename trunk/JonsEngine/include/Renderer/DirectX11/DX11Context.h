#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"

#include <Windows.h>
#include <d3d11.h>

namespace JonsEngine
{
    class DX11Context
    {
    public:
        DX11Context(const HWND windowHandle);
        ~DX11Context();

        D3D11_TEXTURE2D_DESC GetBackbufferTextureDesc() const;


    protected:
        const HWND mWindowHandle;
        IDXGISwapChainPtr mSwapchain;
        ID3D11DevicePtr mDevice;
        ID3D11DeviceContextPtr mContext;
        DXGI_SWAP_CHAIN_DESC mSwapchainDesc;
    };
}

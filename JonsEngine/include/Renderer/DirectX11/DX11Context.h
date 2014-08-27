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


    protected:
        const HWND mWindowHandle;
        IDXGISwapChainPtr mSwapchain;
        ID3D11DevicePtr mDevice;
        ID3D11DeviceContextPtr mContext;
        DXGI_SWAP_CHAIN_DESC mSwapchainDesc;
    };
}

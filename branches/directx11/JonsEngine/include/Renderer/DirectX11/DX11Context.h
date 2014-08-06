#pragma once

#include <Windows.h>
#include <d3d11.h>

namespace JonsEngine
{
    class DX11Context
    {
    public:
        DX11Context(const HWND windowHandle);
        ~DX11Context();
        //ID3D11Debug* dbg;

    protected:
        const HWND mWindowHandle;
        IDXGISwapChain* mSwapchain;
        ID3D11Device* mDevice;
        ID3D11DeviceContext* mContext;
        DXGI_SWAP_CHAIN_DESC mSwapchainDesc;
    };
}
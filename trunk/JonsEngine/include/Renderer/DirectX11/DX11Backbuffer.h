#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11Backbuffer
    {
    public:
        DX11Backbuffer(ID3D11DevicePtr context, IDXGISwapChainPtr swapchain, ID3D11DepthStencilViewPtr depthBuffer, uint32_t textureWidth, uint32_t textureHeight);
        ~DX11Backbuffer();

        void ClearBackbuffer(ID3D11DeviceContextPtr context);
        void BindForShadingStage(ID3D11DeviceContextPtr context);
        void CopyBackbufferTexture(ID3D11Texture2DPtr dest);


    private:
        ID3D11Texture2DPtr mBackbufferTexture;
        ID3D11RenderTargetViewPtr mBackbufferRTV;
        ID3D11DepthStencilViewPtr mDepthbuffer;
        ID3D11DepthStencilStatePtr mDepthStencilState;
    };
}

#pragma once

#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11BackBuffer
    {
    public:
        DX11BackBuffer(ID3D11Device* context, IDXGISwapChain* swapchain, uint32_t textureWidth, uint32_t textureHeight);
        ~DX11BackBuffer();

        void BindForShadingStage(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthBuffer);


    private:
        ID3D11RenderTargetView* mBackbuffer;
        ID3D11DepthStencilState* mDepthStencilState;
    };
}
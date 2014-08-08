#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11Backbuffer
    {
    public:
        DX11Backbuffer(ID3D11Device* context, IDXGISwapChain* swapchain, uint32_t textureWidth, uint32_t textureHeight);
        ~DX11Backbuffer();

        void ClearBackbuffer(ID3D11DeviceContext* context);
        void BindForShadingStage(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthBuffer);


    private:
        ID3D11RenderTargetViewPtr mBackbuffer;
        ID3D11DepthStencilStatePtr mDepthStencilState;
    };
}

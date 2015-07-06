#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11FullscreenTrianglePass;

    class DX11Backbuffer
    {
    public:
        DX11Backbuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, IDXGISwapChainPtr swapchain, DX11FullscreenTrianglePass& fullscreenPass);
        ~DX11Backbuffer();

        void FillBackbuffer();
        void CopyBackbuffer(ID3D11Texture2DPtr dest);
        void BindForDrawing(ID3D11DepthStencilViewPtr dsv, const bool renderToSRGB);
        
        void ClearBackbuffer(const DX11Color& clearColor);


    private:
        ID3D11DeviceContextPtr mContext;
        DX11Texture mTexture;
        // for sRGB post-processing rendering to the backbuffer
        ID3D11RenderTargetViewPtr mRTV;
        // used to write from accumulationbuffer to backbuffer; performs linear->sRGB conversion automatically
        ID3D11RenderTargetViewPtr mRTV_SRGB;
        ID3D11PixelShaderPtr mPixelShader;

        DX11FullscreenTrianglePass& mFullscreenPass;
    };
}

#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11Backbuffer
    {
    public:
        DX11Backbuffer(ID3D11DevicePtr context, IDXGISwapChainPtr swapchain);
        ~DX11Backbuffer();

        void FillBackbuffer(ID3D11DeviceContextPtr context, const bool convertToSRGB);
        
        void ClearBackbuffer(ID3D11DeviceContextPtr context);
        void CopyBackbufferTexture(ID3D11DeviceContextPtr context, ID3D11Texture2DPtr dest);


    private:
        ID3D11Texture2DPtr mBackbufferTexture;
        // for sRGB post-processing rendering to the backbuffer
        ID3D11RenderTargetViewPtr mRTV;
        // used to write from accumulationbuffer to backbuffer; performs linear->sRGB conversion automatically
        ID3D11RenderTargetViewPtr mRTV_SRGB;
    };
}

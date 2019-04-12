#pragma once

#include "DX11Utils.h"
#include "Types.h"
#include "Directx11.h"

namespace JonsEngine
{
    class DX11Backbuffer
    {
    public:
        DX11Backbuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, IDXGISwapChainPtr swapchain);
        ~DX11Backbuffer();

		ID3D11Texture2DPtr GetDepthbuffer();

        void CopyBackbuffer(ID3D11Texture2DPtr dest);

		void BindForTonemapping();
        void BindForPostProcessing();
        
        void ClearBackbuffer(const DX11Color& clearColor);


    private:
        ID3D11DeviceContextPtr mContext;
        ID3D11Texture2DPtr mBackbufferTexture;
		ID3D11Texture2DPtr mDepthStencilBuffer;
        // for sRGB post-processing rendering to the backbuffer
        ID3D11RenderTargetViewPtr mRTV;
        // used to write from accumulationbuffer to backbuffer; performs linear->sRGB conversion automatically
        ID3D11RenderTargetViewPtr mRTV_SRGB;
    };
}

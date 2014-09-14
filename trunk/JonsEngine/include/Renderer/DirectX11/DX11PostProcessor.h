#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    class DX11FullscreenTrianglePass;
    class DX11Backbuffer;

    class DX11PostProcessor
    {
    public:
        DX11PostProcessor(ID3D11DevicePtr device, DX11FullscreenTrianglePass& fullscreenPass, D3D11_TEXTURE2D_DESC& backbufferTextureDesc);
        ~DX11PostProcessor();

        void FXAAPass(ID3D11DeviceContextPtr context, DX11Backbuffer& backbuffer);


    private:
        DX11FullscreenTrianglePass& mFullscreenPass;
        ID3D11Texture2DPtr mTexture;
        ID3D11ShaderResourceViewPtr mSRV;
        ID3D11RenderTargetViewPtr mRTV;
        ID3D11PixelShaderPtr mFXAAPixelShader;
    };
}
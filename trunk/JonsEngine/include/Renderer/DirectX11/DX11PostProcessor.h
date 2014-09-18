#pragma once

#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    class DX11FullscreenTrianglePass;
    class DX11Backbuffer;

    class DX11PostProcessor
    {
    public:
        DX11PostProcessor(ID3D11DevicePtr device, DX11FullscreenTrianglePass& fullscreenPass, D3D11_TEXTURE2D_DESC backbufferTextureDesc);
        ~DX11PostProcessor();

        void FXAAPass(ID3D11DeviceContextPtr context, DX11Backbuffer& backbuffer, const Vec2& screenSize);


    private:
        struct FXAACBuffer
        {
            Vec2 mScreenSize;
            float __padding[2];


            FXAACBuffer(const Vec2& screenSize) : mScreenSize(1.0f / screenSize.x, 1.0f / screenSize.y)
            {
            }
        };

        DX11FullscreenTrianglePass& mFullscreenPass;
        DX11ConstantBuffer<FXAACBuffer> mFXAACBuffer;
        ID3D11Texture2DPtr mTexture;
        ID3D11ShaderResourceViewPtr mSRV;
        ID3D11RenderTargetViewPtr mRTV;
        ID3D11PixelShaderPtr mFXAAPixelShader;
    };
}
#pragma once

#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11BoxBlurPass.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    class DX11FullscreenTrianglePass;
    class DX11Backbuffer;

    class DX11PostProcessor
    {
    public:
        DX11PostProcessor(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11FullscreenTrianglePass& fullscreenPass, D3D11_TEXTURE2D_DESC backbufferTextureDesc);
        ~DX11PostProcessor();

        void FXAAPass(DX11Backbuffer& backbuffer, const Vec2& screenSize);


    private:
        struct FXAACBuffer
        {
            Vec2 mScreenSizeReciprocal;
            float __padding[2];


            FXAACBuffer(const Vec2& screenSize) : mScreenSizeReciprocal(1.0f / screenSize.x, 1.0f / screenSize.y)
            {
            }
        };

        ID3D11DeviceContextPtr mContext;
        DX11FullscreenTrianglePass& mFullscreenPass;

        // fxaa
        DX11ConstantBuffer<FXAACBuffer> mFXAACBuffer;
        ID3D11Texture2DPtr mFXAABackbufferTexture;
        ID3D11ShaderResourceViewPtr mFXAASRV;
        ID3D11RenderTargetViewPtr mFXAARTV;
        ID3D11PixelShaderPtr mFXAAPixelShader;
    };
}
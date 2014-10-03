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
        DX11PostProcessor(ID3D11DevicePtr device, DX11FullscreenTrianglePass& fullscreenPass, D3D11_TEXTURE2D_DESC backbufferTextureDesc);
        ~DX11PostProcessor();

        void FXAAPass(ID3D11DeviceContextPtr context, DX11Backbuffer& backbuffer, const Vec2& screenSize);
        void SSAOPass(ID3D11DeviceContextPtr context, const Mat4& camViewProjMatrix, const Mat4& projMatrix, const Mat4& viewMatrix, const Vec2& screenSize);


    private:
        struct FXAACBuffer
        {
            Vec2 mScreenSizeReciprocal;
            float __padding[2];


            FXAACBuffer(const Vec2& screenSize) : mScreenSizeReciprocal(1.0f / screenSize.x, 1.0f / screenSize.y)
            {
            }
        };

        struct SSAOCBuffer
        {
            Mat4 mCamViewProjMatrix;
            Mat4 mProjMatrix;
            Mat4 mViewMatrix;
            Vec2 mScreenSize;
            float __padding[2];


            SSAOCBuffer(const Mat4& camViewProjMatrix, const Mat4& projMatrix, const Mat4& viewMatrix, const Vec2& screenSize) :
                mCamViewProjMatrix(camViewProjMatrix), mProjMatrix(projMatrix), mViewMatrix(viewMatrix), mScreenSize(screenSize)
            {
            }
        };

        DX11FullscreenTrianglePass& mFullscreenPass;
        DX11BoxBlurPass mBoxBlurPass;
        DX11ConstantBuffer<FXAACBuffer> mFXAACBuffer;
        DX11ConstantBuffer<SSAOCBuffer> mSSAOCBuffer;
        ID3D11Texture2DPtr mTexture;
        ID3D11ShaderResourceViewPtr mSRV;
        ID3D11RenderTargetViewPtr mRTV;
        ID3D11PixelShaderPtr mFXAAPixelShader;
        ID3D11PixelShaderPtr mSSAOPixelShader;

        ID3D11Texture2DPtr mSAOTexture1;
        ID3D11ShaderResourceViewPtr mSAOSRV1;
        ID3D11RenderTargetViewPtr mSAORTV1;
        ID3D11Texture2DPtr mSAOTexture2;
        ID3D11ShaderResourceViewPtr mSAOSRV2;
        ID3D11RenderTargetViewPtr mSAORTV2;
    };
}
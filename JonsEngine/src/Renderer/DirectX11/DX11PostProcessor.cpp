#include "include/Renderer/DirectX11/DX11PostProcessor.h"

#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11Backbuffer.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/FXAAPixel.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SSAOPixel.h"

namespace JonsEngine
{
    DX11PostProcessor::DX11PostProcessor(ID3D11DevicePtr device, DX11FullscreenTrianglePass& fullscreenPass, D3D11_TEXTURE2D_DESC backbufferTextureDesc) :
        mFullscreenPass(fullscreenPass), mBoxBlurPass(device, fullscreenPass), mFXAACBuffer(device, mFXAACBuffer.CONSTANT_BUFFER_SLOT_PIXEL), mSSAOCBuffer(device, mSSAOCBuffer.CONSTANT_BUFFER_SLOT_PIXEL),
        mTexture(nullptr), mSRV(nullptr), mRTV(nullptr), mFXAAPixelShader(nullptr), mSSAOPixelShader(nullptr)
    {
        backbufferTextureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

        DXCALL(device->CreateTexture2D(&backbufferTextureDesc, NULL, &mTexture));
        DXCALL(device->CreateRenderTargetView(mTexture, NULL, &mRTV));
        DXCALL(device->CreateShaderResourceView(mTexture, NULL, &mSRV));
        DXCALL(device->CreatePixelShader(gFXAAPixelShader, sizeof(gFXAAPixelShader), NULL, &mFXAAPixelShader));
        DXCALL(device->CreatePixelShader(gSSAOPixelShader, sizeof(gSSAOPixelShader), NULL, &mSSAOPixelShader));

        backbufferTextureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
        DXCALL(device->CreateTexture2D(&backbufferTextureDesc, NULL, &mSAOTexture1));
        DXCALL(device->CreateTexture2D(&backbufferTextureDesc, NULL, &mSAOTexture2));
        DXCALL(device->CreateShaderResourceView(mSAOTexture1, NULL, &mSAOSRV1));
        DXCALL(device->CreateShaderResourceView(mSAOTexture2, NULL, &mSAOSRV2));
        DXCALL(device->CreateRenderTargetView(mSAOTexture1, NULL, &mSAORTV1));
        DXCALL(device->CreateRenderTargetView(mSAOTexture2, NULL, &mSAORTV2));
    }

    DX11PostProcessor::~DX11PostProcessor()
    {
    }


    void DX11PostProcessor::FXAAPass(ID3D11DeviceContextPtr context, DX11Backbuffer& backbuffer, const Vec2& screenSize)
    {
        backbuffer.CopyBackbufferTexture(context, mTexture);

        context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_DIFFUSE, 1, &mSRV.p);
        context->PSSetShader(mFXAAPixelShader, NULL, NULL);
        mFXAACBuffer.SetData(FXAACBuffer(screenSize), context);

        mFullscreenPass.Render(context);
    }

    void DX11PostProcessor::SSAOPass(ID3D11DeviceContextPtr context, const Mat4& camViewProjMatrix, const Mat4& projMatrix, const Mat4& viewMatrix, const Vec2& screenSize)
    {
        ID3D11RenderTargetViewPtr prevRTV = nullptr;
        ID3D11DepthStencilViewPtr prevDSV = nullptr;
        context->OMGetRenderTargets(1, &prevRTV, &prevDSV);

        // pass 1: render AO to texture
        context->OMSetRenderTargets(1, &mSAORTV1.p, NULL);
        context->PSSetShader(mSSAOPixelShader, NULL, NULL);
        mSSAOCBuffer.SetData(SSAOCBuffer(camViewProjMatrix, projMatrix, viewMatrix, screenSize), context);

        mFullscreenPass.Render(context);

        // pass 2: horizontal + vertical blur
        // restore rendering previous render target
        context->OMSetRenderTargets(1, &mSAORTV2.p, NULL);
        mBoxBlurPass.Render(context, mSAOSRV1, Vec2(1.0f / screenSize.x, 0.0f));

        context->OMSetRenderTargets(1, &prevRTV.p, prevDSV);
        mBoxBlurPass.Render(context, mSAOSRV2, Vec2(0.0f, 1.0f / screenSize.y));

    }
}
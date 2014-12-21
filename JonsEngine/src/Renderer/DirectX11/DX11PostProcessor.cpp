#include "include/Renderer/DirectX11/DX11PostProcessor.h"

#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11Backbuffer.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/FXAAPixel.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SSAOPixel.h"

namespace JonsEngine
{
    DX11PostProcessor::DX11PostProcessor(ID3D11DevicePtr device, DX11FullscreenTrianglePass& fullscreenPass, D3D11_TEXTURE2D_DESC backbufferTextureDesc) :
        mFullscreenPass(fullscreenPass), mFXAACBuffer(device, mFXAACBuffer.CONSTANT_BUFFER_SLOT_PIXEL), mFXAABackbufferTexture(nullptr), mFXAARTV(nullptr), mFXAASRV(nullptr), mFXAAPixelShader(nullptr)
    {
        backbufferTextureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

        DXCALL(device->CreateTexture2D(&backbufferTextureDesc, NULL, &mFXAABackbufferTexture));
        DXCALL(device->CreateRenderTargetView(mFXAABackbufferTexture, NULL, &mFXAARTV));
        DXCALL(device->CreateShaderResourceView(mFXAABackbufferTexture, NULL, &mFXAASRV));
        DXCALL(device->CreatePixelShader(gFXAAPixelShader, sizeof(gFXAAPixelShader), NULL, &mFXAAPixelShader));
    }

    DX11PostProcessor::~DX11PostProcessor()
    {
    }


    void DX11PostProcessor::FXAAPass(ID3D11DeviceContextPtr context, DX11Backbuffer& backbuffer, const Vec2& screenSize)
    {
        backbuffer.CopyBackbuffer(mFXAABackbufferTexture);

        context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_EXTRA, 1, &mFXAASRV.p);
        context->PSSetShader(mFXAAPixelShader, NULL, NULL);
        mFXAACBuffer.SetData(FXAACBuffer(screenSize), context);

        mFullscreenPass.Render(context);
    }
}
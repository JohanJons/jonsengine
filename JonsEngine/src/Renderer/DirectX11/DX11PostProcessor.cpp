#include "include/Renderer/DirectX11/DX11PostProcessor.h"

#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11Backbuffer.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/FXAAPixel.h"

namespace JonsEngine
{
    DX11PostProcessor::DX11PostProcessor(ID3D11DevicePtr device, DX11FullscreenTrianglePass& fullscreenPass, D3D11_TEXTURE2D_DESC backbufferTextureDesc) :
        mFullscreenPass(fullscreenPass), mCBuffer(device, mCBuffer.CONSTANT_BUFFER_SLOT_PIXEL), mTexture(nullptr), mSRV(nullptr), mRTV(nullptr)
    {
        backbufferTextureDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;

        DXCALL(device->CreateTexture2D(&backbufferTextureDesc, NULL, &mTexture));
        DXCALL(device->CreateRenderTargetView(mTexture, NULL, &mRTV));
        DXCALL(device->CreateShaderResourceView(mTexture, NULL, &mSRV));
        DXCALL(device->CreatePixelShader(gFXAAPixelShader, sizeof(gFXAAPixelShader), NULL, &mFXAAPixelShader));
    }

    DX11PostProcessor::~DX11PostProcessor()
    {
    }


    void DX11PostProcessor::FXAAPass(ID3D11DeviceContextPtr context, DX11Backbuffer& backbuffer, const Vec2& screenSize)
    {
        backbuffer.CopyBackbufferTexture(context, mTexture);

        context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_DIFFUSE, 1, &mSRV.p);
        context->PSSetShader(mFXAAPixelShader, NULL, NULL);
        mCBuffer.SetData(PostProcessorCBuffer(screenSize), context);

        mFullscreenPass.Render(context);
    }
}
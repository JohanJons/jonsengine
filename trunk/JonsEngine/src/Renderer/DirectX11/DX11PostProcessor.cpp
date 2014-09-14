#include "include/Renderer/DirectX11/DX11PostProcessor.h"

#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11Backbuffer.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/FXAAPixel.h"

namespace JonsEngine
{
    DX11PostProcessor::DX11PostProcessor(ID3D11DevicePtr device, DX11FullscreenTrianglePass& fullscreenPass, D3D11_TEXTURE2D_DESC& backbufferTextureDesc) :
        mFullscreenPass(fullscreenPass), mTexture(nullptr), mSRV(nullptr), mRTV(nullptr)
    {
        DXCALL(device->CreateTexture2D(&backbufferTextureDesc, NULL, &mTexture));
        DXCALL(device->CreateRenderTargetView(mTexture, NULL, &mRTV));
        DXCALL(device->CreateShaderResourceView(mTexture, NULL, &mSRV));
        DXCALL(device->CreatePixelShader(gFXAAPixelShader, sizeof(gFXAAPixelShader), NULL, &mFXAAPixelShader));
    }

    DX11PostProcessor::~DX11PostProcessor()
    {
    }


    void DX11PostProcessor::FXAAPass(ID3D11DeviceContextPtr context, DX11Backbuffer& backbuffer)
    {


        context->PSSetShader(mFXAAPixelShader, NULL, NULL);

        mFullscreenPass.Render(context);
    }
}
#include "include/Renderer/DirectX11/DX11Backbuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11LightAccumulationbuffer.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SimpleTexturePixel.h"


namespace JonsEngine
{
    const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11Backbuffer::DX11Backbuffer(ID3D11DevicePtr device, IDXGISwapChainPtr swapchain) : mBackbufferTexture(nullptr), mRTV(nullptr), mRTV_SRGB(nullptr)
    {
        // backbuffer rendertarget setup
        DXCALL(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&mBackbufferTexture));
        
        // create RTVs
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
        ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0;
        DXCALL(device->CreateRenderTargetView(mBackbufferTexture, &rtvDesc, &mRTV));

        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        DXCALL(device->CreateRenderTargetView(mBackbufferTexture, &rtvDesc, &mRTV_SRGB));

        // pixelshader that will output lightAccumBuffer to backbuffer
        //DXCALL(device->CreatePixelShader(gSimpleTexturePixelShader, sizeof(gSimpleTexturePixelShader), NULL, &mPixelShader));
    }

    DX11Backbuffer::~DX11Backbuffer()
    {
    }


    void DX11Backbuffer::ClearBackbuffer(ID3D11DeviceContextPtr context)
    {
        context->ClearRenderTargetView(mRTV, gClearColor);
    }

    void DX11Backbuffer::FillBackbuffer(ID3D11DeviceContextPtr context, const bool convertToSRGB)
    {
        if (convertToSRGB)
            context->OMSetRenderTargets(1, &mRTV_SRGB.p, NULL);
        else
            context->OMSetRenderTargets(1, &mRTV.p, NULL);

        // ...... mLightAccumulationBuffer.BindForReading(context);
        //context->PSSetShader(mPixelShader, NULL, NULL);
        
        // ...... mFullscreenPass.Render(context);
    }

    void DX11Backbuffer::CopyBackbufferTexture(ID3D11DeviceContextPtr context, ID3D11Texture2DPtr dest)
    {
        context->CopyResource(dest, mBackbufferTexture);
    }
}
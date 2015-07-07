#include "include/Renderer/DirectX11/DX11Backbuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SimpleTexturePixel.h"


namespace JonsEngine
{
    const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11Backbuffer::DX11Backbuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, IDXGISwapChainPtr swapchain, DX11FullscreenTrianglePass& fullscreenPass) :
        mContext(context),
        mTexture(swapchain, context),
        mRTV(mTexture.CreateRTV(device, DXGI_FORMAT_R8G8B8A8_UNORM)),
        mRTV_SRGB(mTexture.CreateRTV(device, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)),
        mFullscreenPass(fullscreenPass)
    {
        // pixelshader that will output lightAccumBuffer to backbuffer
        DXCALL(device->CreatePixelShader(gSimpleTexturePixelShader, sizeof(gSimpleTexturePixelShader), nullptr, &mPixelShader))
    }

    DX11Backbuffer::~DX11Backbuffer()
    {
    }


    void DX11Backbuffer::FillBackbuffer()
    {
        mContext->PSSetShader(mPixelShader, nullptr, 0);
        mFullscreenPass.Render();
    }

    void DX11Backbuffer::CopyBackbuffer(ID3D11Texture2DPtr dest)
    {
        mTexture.CopyTexture(dest);
    }

    void DX11Backbuffer::BindForDrawing(ID3D11DepthStencilViewPtr dsv, const bool renderToSRGB)
    {
        if (renderToSRGB)
            mContext->OMSetRenderTargets(1, &mRTV_SRGB.p, dsv);
        else
            mContext->OMSetRenderTargets(1, &mRTV.p, dsv);
    }


    void DX11Backbuffer::ClearBackbuffer(const DX11Color& clearColor)
    {
        mContext->ClearRenderTargetView(mRTV, &clearColor[0]);
    }
}
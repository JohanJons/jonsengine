#include "include/Renderer/DirectX11/DX11Backbuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/SimpleTexturePixel.h"


namespace JonsEngine
{
    static const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11Backbuffer::DX11Backbuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, IDXGISwapChainPtr swapchain, DX11FullscreenTrianglePass& fullscreenPass) :
        mContext(context),
        mBackbufferTexture(nullptr),
        mRTV(nullptr),
        mRTV_SRGB(nullptr),
        mFullscreenPass(fullscreenPass)
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
        mContext->CopyResource(dest, mBackbufferTexture);
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
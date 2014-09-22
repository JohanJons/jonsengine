#include "include/Renderer/DirectX11/DX11Backbuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"


namespace JonsEngine
{
    const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11Backbuffer::DX11Backbuffer(ID3D11DevicePtr device, IDXGISwapChainPtr swapchain, uint32_t textureWidth, uint32_t textureHeight) : 
        mBackbufferTexture(nullptr), mBackbufferRTV(nullptr), mDepthStencilBuffer(nullptr), mDepthStencilView(nullptr), mDepthSRV(nullptr), mDepthStencilState(nullptr)
    {
        // backbuffer rendertarget setup
        DXCALL(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&mBackbufferTexture));
        DXCALL(device->CreateRenderTargetView(mBackbufferTexture, NULL, &mBackbufferRTV));

        // create depth buffer/view/srv
        D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
        ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
        depthStencilBufferDesc.ArraySize = 1;
        depthStencilBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
        depthStencilBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
        depthStencilBufferDesc.Width = textureWidth;
        depthStencilBufferDesc.Height = textureHeight;
        depthStencilBufferDesc.MipLevels = 1;
        depthStencilBufferDesc.SampleDesc.Count = 1;
        depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        DXCALL(device->CreateTexture2D(&depthStencilBufferDesc, NULL, &mDepthStencilBuffer));

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;
        DXCALL(device->CreateShaderResourceView(mDepthStencilBuffer, &srvDesc, &mDepthSRV));

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        DXCALL(device->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, &mDepthStencilView));

        // depth stencil config used in shading stage
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = false;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilDesc.StencilEnable = false;
        DXCALL(device->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));
    }

    DX11Backbuffer::~DX11Backbuffer()
    {
    }


    void DX11Backbuffer::ClearBackbuffer(ID3D11DeviceContextPtr context)
    {
        context->ClearRenderTargetView(mBackbufferRTV, gClearColor);
    }

    void DX11Backbuffer::ClearStencilBuffer(ID3D11DeviceContextPtr context)
    {
        context->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0);
    }

    void DX11Backbuffer::BindForShadingStage(ID3D11DeviceContextPtr context, const bool useDepthAsSRV)
    {
        // set backbuffer as rendertarget
        if (useDepthAsSRV)
        {
            context->OMSetRenderTargets(1, &mBackbufferRTV.p, NULL);
            context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_DEPTH, 1, &mDepthSRV.p);
        }
        else
        {
            context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_DEPTH, 1, &gNullSrv.p);
            context->OMSetRenderTargets(1, &mBackbufferRTV.p, mDepthStencilView);
        }

        // disable further depth testing/writing
        context->OMSetDepthStencilState(mDepthStencilState, 0);
    }

    void DX11Backbuffer::CopyBackbufferTexture(ID3D11DeviceContextPtr context, ID3D11Texture2DPtr dest)
    {
        context->CopyResource(dest, mBackbufferTexture);
    }


    ID3D11DepthStencilViewPtr DX11Backbuffer::GetDepthStencilView()
    {
        return mDepthStencilView;
    }
}

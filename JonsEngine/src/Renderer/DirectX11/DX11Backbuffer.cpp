#include "include/Renderer/DirectX11/DX11Backbuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11Backbuffer::DX11Backbuffer(ID3D11DevicePtr device, IDXGISwapChainPtr swapchain, ID3D11DepthStencilViewPtr depthBuffer, uint32_t textureWidth, uint32_t textureHeight) : 
        mBackbufferTexture(nullptr), mBackbufferRTV(nullptr), mDepthbuffer(depthBuffer), mDepthStencilState(nullptr)
    {
        // backbuffer rendertarget setup
        DXCALL(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&mBackbufferTexture));
        DXCALL(device->CreateRenderTargetView(mBackbufferTexture, NULL, &mBackbufferRTV));

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

    void DX11Backbuffer::BindForShadingStage(ID3D11DeviceContextPtr context)
    {
        // set backbuffer as rendertarget and bind gbuffer textures as texture inputs
        context->OMSetRenderTargets(1, &mBackbufferRTV.p, mDepthbuffer);

        // disable further depth testing/writing
        context->OMSetDepthStencilState(mDepthStencilState, 0);
    }

    void DX11Backbuffer::CopyBackbufferTexture(ID3D11DeviceContextPtr context, ID3D11Texture2DPtr dest)
    {
        context->CopyResource(dest, mBackbufferTexture);
    }
}

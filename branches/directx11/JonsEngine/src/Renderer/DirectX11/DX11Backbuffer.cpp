#include "include/Renderer/DirectX11/DX11Backbuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11Backbuffer::DX11Backbuffer(ID3D11DevicePtr device, IDXGISwapChainPtr swapchain, uint32_t textureWidth, uint32_t textureHeight) : mBackbuffer(nullptr), mDepthStencilState(nullptr)
    {
        // backbuffer rendertarget setup
        ID3D11Texture2DPtr backbuffer = nullptr;
        DXCALL(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer));
        DXCALL(device->CreateRenderTargetView(backbuffer, NULL, &mBackbuffer));

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
        context->ClearRenderTargetView(mBackbuffer, gClearColor);
    }

    void DX11Backbuffer::BindForShadingStage(ID3D11DeviceContextPtr context, ID3D11DepthStencilViewPtr depthBuffer)
    {
        // set backbuffer as rendertarget and bind gbuffer textures as texture inputs
        context->OMSetRenderTargets(1, &mBackbuffer.p, depthBuffer);

        // disable further depth testing/writing
        context->OMSetDepthStencilState(mDepthStencilState, 0);
    }
}

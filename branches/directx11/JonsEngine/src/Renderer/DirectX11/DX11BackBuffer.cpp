#include "include/Renderer/DirectX11/DX11BackBuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11BackBuffer::DX11BackBuffer(ID3D11Device* device, IDXGISwapChain* swapchain, uint32_t textureWidth, uint32_t textureHeight) : mBackbuffer(nullptr), mDepthStencilState(nullptr)
    {
        // backbuffer rendertarget setup
        ID3D11Texture2D* backbuffer = nullptr;
        DXCALL(swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer));
        DXCALL(device->CreateRenderTargetView(backbuffer, NULL, &mBackbuffer));
        backbuffer->Release();

        // depth stencil config used in shading stage
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = false;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilDesc.StencilEnable = false;
        DXCALL(device->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));
    }

    DX11BackBuffer::~DX11BackBuffer()
    {
        mBackbuffer->Release();
        mDepthStencilState->Release();
    }


    void DX11BackBuffer::ClearBackbuffer(ID3D11DeviceContext* context)
    {
        context->ClearRenderTargetView(mBackbuffer, gClearColor);
    }

    void DX11BackBuffer::BindForShadingStage(ID3D11DeviceContext* context, ID3D11DepthStencilView* depthBuffer)
    {
        // set backbuffer as rendertarget and bind gbuffer textures as texture inputs
        context->OMSetRenderTargets(1, &mBackbuffer, depthBuffer);

        // disable further depth testing/writing
        context->OMSetDepthStencilState(mDepthStencilState, 0);
    }
}
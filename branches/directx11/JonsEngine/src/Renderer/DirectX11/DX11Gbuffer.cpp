#include "include/Renderer/DirectX11/DX11GBuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


    GBuffer::GBuffer(ID3D11Device* device, IDXGISwapChain* swapChain)
    {
        // get backbuffer width/height
        DXGI_SWAP_CHAIN_DESC swapChainDesc;
        ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
        DXCALL(swapChain->GetDesc(&swapChainDesc));

        // create gbuffer textures/rendertargets
        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        textureDesc.Width = swapChainDesc.BufferDesc.Width;
        textureDesc.Height = swapChainDesc.BufferDesc.Height;
        textureDesc.ArraySize = 1;
        textureDesc.MipLevels = 1;
        textureDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        
        for (uint32_t index = 0; index < GBuffer::GBUFFER_NUM_RENDERTARGETS; index++) 
        {
            DXCALL(device->CreateTexture2D(&textureDesc, NULL, &mGeometryTextures[index]));
            DXCALL(device->CreateRenderTargetView(mGeometryTextures[index], NULL, &mRenderTargets[index]));
        }

        // create depth buffer/view
        D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
        ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
        depthStencilBufferDesc.ArraySize = 1;
        depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilBufferDesc.Width = swapChainDesc.BufferDesc.Width;
        depthStencilBufferDesc.Height = swapChainDesc.BufferDesc.Height;
        depthStencilBufferDesc.MipLevels = 1;
        depthStencilBufferDesc.SampleDesc.Count = 1;
        depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        DXCALL(device->CreateTexture2D(&depthStencilBufferDesc, NULL, &mDepthStencilBuffer));
        DXCALL(device->CreateDepthStencilView(mDepthStencilBuffer, NULL, &mDepthStencilView));

        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilDesc.StencilEnable = false;
        DXCALL(device->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));
    }

    GBuffer::~GBuffer()
    {
        for (uint32_t index = 0; index < GBuffer::GBUFFER_NUM_RENDERTARGETS; index++)
        {
            mGeometryTextures[index]->Release();
            mRenderTargets[index]->Release();
        }

        mDepthStencilBuffer->Release();
        mDepthStencilView->Release();
        mDepthStencilState->Release();
    }


    void GBuffer::BindForDrawing(ID3D11DeviceContext* context)
    {
        for (uint32_t index = 0; index < GBuffer::GBUFFER_NUM_RENDERTARGETS; index++)
            context->ClearRenderTargetView(mRenderTargets[index], gClearColor);
        context->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        context->OMSetRenderTargets(GBuffer::GBUFFER_NUM_RENDERTARGETS, &mRenderTargets[0], mDepthStencilView);
        context->OMSetDepthStencilState(mDepthStencilState, 1);
    }

    void GBuffer::BindForReading(ID3D11DeviceContext* context)
    {

    }
}
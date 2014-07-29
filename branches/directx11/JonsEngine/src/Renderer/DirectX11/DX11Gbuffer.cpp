#include "include/Renderer/DirectX11/DX11GBuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/GBufferVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/GBufferPixel.h"

namespace JonsEngine
{
    const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11GBuffer::DX11GBuffer(ID3D11Device* device, uint32_t textureWidth, uint32_t textureHeight) : mDepthStencilBuffer(nullptr), mDepthStencilView(nullptr), mDepthStencilState(nullptr),
        mInputLayout(nullptr), mVertexShader(nullptr), mPixelShader(nullptr), mConstantBuffer(device)
    {
        // create gbuffer textures/rendertargets
        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        textureDesc.Width = textureWidth;
        textureDesc.Height = textureHeight;
        textureDesc.ArraySize = 1;
        textureDesc.MipLevels = 1;
        textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        
        for (uint32_t index = 0; index < DX11GBuffer::GBUFFER_NUM_RENDERTARGETS; index++)
        {
            DXCALL(device->CreateTexture2D(&textureDesc, NULL, &mTextures[index]));
            DXCALL(device->CreateRenderTargetView(mTextures[index], NULL, &mRenderTargets[index]));
            DXCALL(device->CreateShaderResourceView(mTextures[index], NULL, &mShaderResourceViews[index]));
        }

        // create depth buffer/view
        D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
        ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
        depthStencilBufferDesc.ArraySize = 1;
        depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilBufferDesc.Width = textureWidth;
        depthStencilBufferDesc.Height = textureHeight;
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

        // input layout
        D3D11_INPUT_ELEMENT_DESC inputDescription[2];
        ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC)* 2);
        inputDescription[0].SemanticName = "POSITION";
        inputDescription[0].SemanticIndex = 0;
        inputDescription[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription[0].InputSlot = 0;
        inputDescription[0].AlignedByteOffset = 0;
        inputDescription[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[0].InstanceDataStepRate = 0;
        inputDescription[1].SemanticName = "TEXCOORD";
        inputDescription[1].SemanticIndex = 0;
        inputDescription[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        inputDescription[1].InputSlot = 1;
        inputDescription[1].AlignedByteOffset = 0;
        inputDescription[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[1].InstanceDataStepRate = 0;
        DXCALL(device->CreateInputLayout(inputDescription, 2, gGBufferVertexShader, sizeof(gGBufferVertexShader), &mInputLayout));

        // create shader objects
        DXCALL(device->CreateVertexShader(gGBufferVertexShader, sizeof(gGBufferVertexShader), NULL, &mVertexShader));
        DXCALL(device->CreatePixelShader(gGBufferPixelShader, sizeof(gGBufferPixelShader), NULL, &mPixelShader));
    }

    DX11GBuffer::~DX11GBuffer()
    {
        for (uint32_t index = 0; index < DX11GBuffer::GBUFFER_NUM_RENDERTARGETS; index++)
        {
            mTextures[index]->Release();
            mRenderTargets[index]->Release();
            mShaderResourceViews[index]->Release();
        }

        mDepthStencilBuffer->Release();
        mDepthStencilView->Release();
        mDepthStencilState->Release();
        mInputLayout->Release();
        mVertexShader->Release();
        mPixelShader->Release();
    }


    void DX11GBuffer::SetConstantData(ID3D11DeviceContext* context, const Mat4& wvpMatrix, const Mat4& worldMatrix, const float textureTilingFactor, const bool hasDiffuseTexture)
    {
        mConstantBuffer.SetData({wvpMatrix, worldMatrix, textureTilingFactor, hasDiffuseTexture, false}, context, 0);
    }

    void DX11GBuffer::BindForDrawing(ID3D11DeviceContext* context)
    {
        for (uint32_t index = 0; index < DX11GBuffer::GBUFFER_NUM_RENDERTARGETS; index++)
            context->ClearRenderTargetView(mRenderTargets[index], gClearColor);
        context->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        context->OMSetRenderTargets(DX11GBuffer::GBUFFER_NUM_RENDERTARGETS, &mRenderTargets[0], mDepthStencilView);
        context->OMSetDepthStencilState(mDepthStencilState, 1);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        
        context->IASetInputLayout(mInputLayout);

        context->VSSetShader(mVertexShader, NULL, NULL);
        context->PSSetShader(mPixelShader, NULL, NULL);
    }

    void DX11GBuffer::BindForReading(ID3D11DeviceContext* context)
    {
        for (uint32_t index = 0; index < DX11GBuffer::GBUFFER_NUM_RENDERTARGETS; index++)
            context->PSSetShaderResources(index, 1, &mShaderResourceViews[index]);
    }
}
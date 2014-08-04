#include "include/Renderer/DirectX11/DX11GBuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/GBufferVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/GBufferPixel.h"

namespace JonsEngine
{
    const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11GBuffer::DX11GBuffer(ID3D11Device* device, uint32_t textureWidth, uint32_t textureHeight) : mDepthStencilBuffer(nullptr), mDepthStencilView(nullptr),
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

        // input layout
        D3D11_INPUT_ELEMENT_DESC inputDescription[DX11Mesh::NUM_VERTEX_BUFFER_SLOTS];
        ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC) * DX11Mesh::NUM_VERTEX_BUFFER_SLOTS);
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_VERTICES].SemanticName = "POSITION";
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_VERTICES].SemanticIndex = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_VERTICES].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_VERTICES].InputSlot = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_VERTICES].AlignedByteOffset = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_VERTICES].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_VERTICES].InstanceDataStepRate = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_NORMALS].SemanticName = "NORMAL";
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_NORMALS].SemanticIndex = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_NORMALS].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_NORMALS].InputSlot = 1;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_NORMALS].AlignedByteOffset = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_NORMALS].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_NORMALS].InstanceDataStepRate = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_TANGENTS].SemanticName = "TANGENT";
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_TANGENTS].SemanticIndex = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_TANGENTS].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_TANGENTS].InputSlot = 2;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_TANGENTS].AlignedByteOffset = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_TANGENTS].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_TANGENTS].InstanceDataStepRate = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_BITANGENTS].SemanticName = "BITANGENT";
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_BITANGENTS].SemanticIndex = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_BITANGENTS].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_BITANGENTS].InputSlot = 3;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_BITANGENTS].AlignedByteOffset = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_BITANGENTS].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_BITANGENTS].InstanceDataStepRate = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_TEXCOORDS].SemanticName = "TEXCOORD";
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_TEXCOORDS].SemanticIndex = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_TEXCOORDS].Format = DXGI_FORMAT_R32G32_FLOAT;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_TEXCOORDS].InputSlot = 4;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_TEXCOORDS].AlignedByteOffset = 0;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_TEXCOORDS].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[DX11Mesh::VERTEX_BUFFER_SLOT_TEXCOORDS].InstanceDataStepRate = 0;
        DXCALL(device->CreateInputLayout(inputDescription, DX11Mesh::NUM_VERTEX_BUFFER_SLOTS, gGBufferVertexShader, sizeof(gGBufferVertexShader), &mInputLayout));

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
        mInputLayout->Release();
        mVertexShader->Release();
        mPixelShader->Release();
    }


    void DX11GBuffer::SetConstantData(ID3D11DeviceContext* context, const Mat4& wvpMatrix, const Mat4& worldMatrix, const float textureTilingFactor, const bool hasDiffuseTexture, const bool hasNormalTexture)
    {
        mConstantBuffer.SetData({wvpMatrix, worldMatrix, textureTilingFactor, hasDiffuseTexture, hasNormalTexture}, context, 0);
    }

    void DX11GBuffer::BindForDrawing(ID3D11DeviceContext* context)
    {
        for (uint32_t index = 0; index < DX11GBuffer::GBUFFER_NUM_RENDERTARGETS; index++)
            context->ClearRenderTargetView(mRenderTargets[index], gClearColor);
        context->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

        // default == depth testing/writing on
        context->OMSetDepthStencilState(NULL, 0);
        context->OMSetRenderTargets(DX11GBuffer::GBUFFER_NUM_RENDERTARGETS, &mRenderTargets[0], mDepthStencilView);
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

    ID3D11DepthStencilView* DX11GBuffer::GetDepthStencilView()
    {
        return mDepthStencilView;
    }
}
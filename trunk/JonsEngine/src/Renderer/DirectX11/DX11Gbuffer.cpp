#include "include/Renderer/DirectX11/DX11GBuffer.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/GBufferVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/GBufferPixel.h"

namespace JonsEngine
{
    const float gClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };


    DX11GBuffer::DX11GBuffer(ID3D11DevicePtr device, ID3D11DepthStencilViewPtr lightAccumDSV, uint32_t textureWidth, uint32_t textureHeight) :
        mInputLayout(nullptr), mVertexShader(nullptr), mPixelShader(nullptr), mConstantBuffer(device, mConstantBuffer.CONSTANT_BUFFER_SLOT_VERTEX), mDSV(lightAccumDSV)
    {
        // create gbuffer textures/rendertargets
        D3D11_TEXTURE2D_DESC textureDesc;
        ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
        textureDesc.Width = textureWidth;
        textureDesc.Height = textureHeight;
        textureDesc.ArraySize = 1;
        textureDesc.MipLevels = 1;
        textureDesc.SampleDesc.Count = 1;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        
        // diffuse texture
        uint32_t gbufferIndex = DX11GBuffer::GBUFFER_RENDERTARGET_INDEX_DIFFUSE;
        textureDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
        DXCALL(device->CreateTexture2D(&textureDesc, NULL, &mTextures.at(gbufferIndex)));
        DXCALL(device->CreateRenderTargetView(mTextures.at(gbufferIndex), NULL, &mRenderTargets.at(gbufferIndex)));
        DXCALL(device->CreateShaderResourceView(mTextures.at(gbufferIndex), NULL, &mShaderResourceViews.at(gbufferIndex)));

        // normal texture
        gbufferIndex = DX11GBuffer::GBUFFER_RENDERTARGET_INDEX_NORMAL;
        textureDesc.Format = DXGI_FORMAT_R16G16B16A16_SNORM;
        DXCALL(device->CreateTexture2D(&textureDesc, NULL, &mTextures.at(gbufferIndex)));
        DXCALL(device->CreateRenderTargetView(mTextures.at(gbufferIndex), NULL, &mRenderTargets.at(gbufferIndex)));
        DXCALL(device->CreateShaderResourceView(mTextures.at(gbufferIndex), NULL, &mShaderResourceViews.at(gbufferIndex)));

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
    }


    void DX11GBuffer::SetConstantData(ID3D11DeviceContextPtr context, const Mat4& wvpMatrix, const Mat4& worldMatrix, const float textureTilingFactor, const bool hasDiffuseTexture, const bool hasNormalTexture)
    {
        mConstantBuffer.SetData({ wvpMatrix, worldMatrix, textureTilingFactor, hasDiffuseTexture, hasNormalTexture }, context);
    }

    void DX11GBuffer::BindForGeometryStage(ID3D11DeviceContextPtr context)
    {
        for (uint32_t index = 0; index < DX11GBuffer::GBUFFER_NUM_RENDERTARGETS; index++)
        {
            // unbind gbuffer textures as input, it is now rendertarget
            context->PSSetShaderResources(index, 1, &gNullSrv.p);
            context->ClearRenderTargetView(mRenderTargets.at(index), gClearColor);
        }
        // backbuffers depth texture might still be bound on input
        context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_DEPTH, 1, &gNullSrv.p);
        context->ClearDepthStencilView(mDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

        // default == depth testing/writing on
        context->OMSetDepthStencilState(NULL, 0);
        context->OMSetRenderTargets(DX11GBuffer::GBUFFER_NUM_RENDERTARGETS, &(mRenderTargets.begin()->p), mDSV);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->IASetInputLayout(mInputLayout);

        context->VSSetShader(mVertexShader, NULL, NULL);
        context->PSSetShader(mPixelShader, NULL, NULL);
    }

    void DX11GBuffer::BindGeometryTextures(ID3D11DeviceContextPtr context)
    {
        for (uint32_t index = 0; index < DX11GBuffer::GBUFFER_NUM_RENDERTARGETS; index++)
            context->PSSetShaderResources(index, 1, &mShaderResourceViews.at(index).p);
    }
}

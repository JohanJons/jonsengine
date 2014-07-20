#include "include/Renderer/DirectX11/DX11Mesh.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    static MeshID gNextMeshID = 1;


    DX11Mesh::DX11Mesh(ID3D11Device* device, const BYTE* shaderBytecode, const uint32_t shaderBytecodeSize, const std::vector<float>& vertexData, const std::vector<float>& normalData,
        const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData, Logger& logger) 
        : mLogger(logger), mMeshID(gNextMeshID++), mNumIndices(indexData.size()), mVertexSize(sizeof(float)* 3), mVertexBuffer(nullptr), mTexcoordBuffer(nullptr), mIndexBuffer(nullptr), mInputLayout(nullptr)
    {
        // vertex buffer
        D3D11_BUFFER_DESC bufferDescription;
        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = D3D11_USAGE_DEFAULT;
        bufferDescription.ByteWidth = vertexData.size() * sizeof(float);
        bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = &vertexData.at(0);

        DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mVertexBuffer));

        // texcoord buffer
        D3D11_BUFFER_DESC bufferDescription;
        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = D3D11_USAGE_DEFAULT;
        bufferDescription.ByteWidth = vertexData.size() * sizeof(float);
        bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = &vertexData.at(0);

        DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mVertexBuffer));

        // index buffer
        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = D3D11_USAGE_DEFAULT;
        bufferDescription.ByteWidth = indexData.size() * sizeof(uint32_t);
        bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = &indexData.at(0);

        DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mIndexBuffer));

        // input layout
        D3D11_INPUT_ELEMENT_DESC inputDescription[2];
        ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC) * 2);
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
        inputDescription[1].InputSlot = 0;
        inputDescription[1].AlignedByteOffset = 12;
        inputDescription[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription[1].InstanceDataStepRate = 0;

        DXCALL(device->CreateInputLayout(inputDescription, 2, shaderBytecode, shaderBytecodeSize, &mInputLayout));
    }
    
    DX11Mesh::~DX11Mesh()
    {
        mVertexBuffer->Release();
        mIndexBuffer->Release();
        mInputLayout->Release();
    }


    void DX11Mesh::Draw(ID3D11DeviceContext* context)
    {
        uint32_t offset = 0;

        context->IASetVertexBuffers(0, 1, &mVertexBuffer, &mVertexSize, &offset);
        context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->IASetInputLayout(mInputLayout);

        context->DrawIndexed(mNumIndices, 0, 0);
    }

    MeshID DX11Mesh::GetMeshID() const
    {
        return mMeshID;
    }
}
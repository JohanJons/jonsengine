#include "include/Renderer/DirectX11/DX11Mesh.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    static MeshID gNextMeshID = 1;

    enum BUFFER_SLOT
    {
        BUFFER_SLOT_VERTEX = 0,
        BUFFER_SLOT_TEXCOORD
    };


    DX11Mesh::DX11Mesh(ID3D11Device* device, const std::vector<float>& vertexData, const std::vector<float>& normalData,
        const std::vector<float>& texCoords, const std::vector<float>& tangentData, const std::vector<float>& bitangentData, const std::vector<uint32_t>& indexData) 
        : mMeshID(gNextMeshID++), mNumIndices(indexData.size()), mVertexBuffer(nullptr), mTexcoordBuffer(nullptr), mIndexBuffer(nullptr)
    {
        // vertex buffer
        D3D11_BUFFER_DESC bufferDescription;
        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDescription.ByteWidth = vertexData.size() * sizeof(float);
        bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = &vertexData.at(0);
        DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mVertexBuffer));

        // normal buffer
        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDescription.ByteWidth = normalData.size() * sizeof(float);
        bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = &normalData.at(0);
        DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mNormalBuffer));

        // tangent buffer
        if (tangentData.size() > 0)
        {
            ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
            bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
            bufferDescription.ByteWidth = tangentData.size() * sizeof(float);
            bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
            initData.pSysMem = &tangentData.at(0);
            DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mTangentBuffer));
        }

        // bitangent buffer
        if (bitangentData.size() > 0)
        {
            ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
            bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
            bufferDescription.ByteWidth = bitangentData.size() * sizeof(float);
            bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
            initData.pSysMem = &bitangentData.at(0);
            DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mBitangentBuffer));
        }

        // texcoord buffer
        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDescription.ByteWidth = texCoords.size() * sizeof(float);
        bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = &texCoords.at(0);
        DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mTexcoordBuffer));

        // index buffer
        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDescription.ByteWidth = indexData.size() * sizeof(uint32_t);
        bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = &indexData.at(0);
        DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mIndexBuffer));
    }
    
    DX11Mesh::~DX11Mesh()
    {
    }


    void DX11Mesh::Draw(ID3D11DeviceContext* context)
    {
        uint32_t vertexSize = sizeof(float) * 3, texcoordSize = sizeof(float) * 2;
        uint32_t offset = 0;

        context->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_VERTICES, 1, &mVertexBuffer.p, &vertexSize, &offset);
        context->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_NORMALS, 1, &mNormalBuffer.p, &vertexSize, &offset);
        if (mTangentBuffer)
            context->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_TANGENTS, 1, &mTangentBuffer.p, &vertexSize, &offset);
        if (mBitangentBuffer)
            context->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_BITANGENTS, 1, &mBitangentBuffer.p, &vertexSize, &offset);
        context->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_TEXCOORDS, 1, &mTexcoordBuffer.p, &texcoordSize, &offset);
        context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        context->DrawIndexed(mNumIndices, 0, 0);
    }

    MeshID DX11Mesh::GetMeshID() const
    {
        return mMeshID;
    }
}

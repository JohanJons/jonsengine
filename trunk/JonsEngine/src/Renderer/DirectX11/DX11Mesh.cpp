#include "include/Renderer/DirectX11/DX11Mesh.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

#include <array>

namespace JonsEngine
{
    static MeshID gNextMeshID = 1;

    const std::array<uint16_t, 16> gAABBIndices = { 0, 1, 2, 3, 0, 5, 4, 3, 2, 7, 4, 7, 6, 5, 6, 1 };


    DX11Mesh::DX11Mesh(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords,
        const std::vector<float>& tangentData, const std::vector<float>& bitangentData, const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds)
        : 
        mContext(context), mVertexBuffer(nullptr), mNormalBuffer(nullptr), mTangentBuffer(nullptr), mBitangentBuffer(nullptr), mTexcoordBuffer(nullptr), mIndexBuffer(nullptr), mMeshID(gNextMeshID++), mNumVertices(vertexData.size()), mNumIndices(indexData.size())
    {
        // vertex buffer
        // use a temporary vector to merge vertices and AABB points
        std::vector<float> tempVertexData(vertexData);
        tempVertexData.push_back(minBounds.x); tempVertexData.push_back(minBounds.y); tempVertexData.push_back(minBounds.z);
        tempVertexData.push_back(minBounds.x); tempVertexData.push_back(maxBounds.y); tempVertexData.push_back(minBounds.z);
        tempVertexData.push_back(minBounds.x); tempVertexData.push_back(maxBounds.y); tempVertexData.push_back(maxBounds.z);
        tempVertexData.push_back(minBounds.x); tempVertexData.push_back(minBounds.y); tempVertexData.push_back(maxBounds.z);
        tempVertexData.push_back(maxBounds.x); tempVertexData.push_back(minBounds.y); tempVertexData.push_back(maxBounds.z);
        tempVertexData.push_back(maxBounds.x); tempVertexData.push_back(minBounds.y); tempVertexData.push_back(minBounds.z);
        tempVertexData.push_back(maxBounds.x); tempVertexData.push_back(maxBounds.y); tempVertexData.push_back(minBounds.z);
        tempVertexData.push_back(maxBounds.x); tempVertexData.push_back(maxBounds.y); tempVertexData.push_back(maxBounds.z);

        D3D11_BUFFER_DESC bufferDescription;
        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDescription.ByteWidth = tempVertexData.size() * sizeof(float);
        bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = &tempVertexData.at(0);
        DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mVertexBuffer));

        // normal buffer
        if (normalData.size() > 0)
        {
            ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
            bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
            bufferDescription.ByteWidth = normalData.size() * sizeof(float);
            bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
            initData.pSysMem = &normalData.at(0);
            DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mNormalBuffer));
        }

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
        if (texCoords.size())
        {
            ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
            bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
            bufferDescription.ByteWidth = texCoords.size() * sizeof(float);
            bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
            initData.pSysMem = &texCoords.at(0);
            DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mTexcoordBuffer));
        }

        // index buffer
        // use a temporary vector to merge vertex indices and AABB indices
        std::vector<uint16_t> tempIndiceData(indexData);
        tempIndiceData.insert(tempIndiceData.end(), gAABBIndices.begin(), gAABBIndices.end());

        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
        bufferDescription.ByteWidth = tempIndiceData.size() * sizeof(uint16_t);
        bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = &tempIndiceData.at(0);
        DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mIndexBuffer));
    }
    
    DX11Mesh::~DX11Mesh()
    {
    }


    void DX11Mesh::Draw()
    {
        const uint32_t vertexSize = sizeof(float) * 3;
        const uint32_t offset = 0;

        mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_VERTICES, 1, &mVertexBuffer.p, &vertexSize, &offset);
        if (mNormalBuffer)
            mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_NORMALS, 1, &mNormalBuffer.p, &vertexSize, &offset);
        if (mTangentBuffer)
            mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_TANGENTS, 1, &mTangentBuffer.p, &vertexSize, &offset);
        if (mBitangentBuffer)
            mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_BITANGENTS, 1, &mBitangentBuffer.p, &vertexSize, &offset);
        if (mTexcoordBuffer)
        {
            const uint32_t texcoordSize = sizeof(float) * 2;
            mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_TEXCOORDS, 1, &mTexcoordBuffer.p, &texcoordSize, &offset);
        }

        mContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        mContext->DrawIndexed(mNumIndices, 0, 0);
    }

    void DX11Mesh::DrawAABB()
    {
        // AABB vertices are offset into the mesh vertex buffer
        const uint32_t vertexSize = 3 * sizeof(float);
        // AABB indices are offset into the mesh index buffer
        const uint32_t offset = mNumVertices * sizeof(float);
        const uint32_t numAABBPoints = gAABBIndices.size();

        mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_VERTICES, 1, &mVertexBuffer.p, &vertexSize, &offset);
        mContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, mNumIndices * sizeof(uint16_t));
        mContext->DrawIndexed(numAABBPoints, 0, 0);
    }

    MeshID DX11Mesh::GetMeshID() const
    {
        return mMeshID;
    }
}

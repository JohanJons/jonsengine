#include "include/Renderer/DirectX11/DX11Mesh.h"

#include "include/Renderer/DirectX11/DX11VertexBufferSet.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"

#include <array>

namespace JonsEngine
{
    const static uint32_t gVertexSize = sizeof(float) * 3;
    const static uint32_t gTangentSize = gVertexSize * 2;
    const static uint32_t gTexcoordSize = sizeof(float) * 2;
    const static uint32_t gBoneIndexSize = sizeof(uint32_t) * MAX_BONES;
    const static uint32_t gBoneWeightSize = sizeof(float) * MAX_BONES;
    const static uint32_t gStaticOffset = 0;
    const std::array<uint16_t, 16> gAABBIndices = { 0, 1, 2, 3, 0, 5, 4, 3, 2, 7, 4, 7, 6, 5, 6, 1 };

    static DX11MeshID gNextMeshID = 1;

    void AppendAABBVertices(std::vector<float>& vertexContainer, const Vec3& minBounds, const Vec3& maxBounds);


    DX11Mesh::DX11Mesh(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords,
        const std::vector<float>& tangentData, const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds) :
        DX11Mesh(device, context, vertexData, normalData, texCoords, tangentData, std::vector<Mat4>(), std::vector<uint16_t>(), std::vector<float>(), indexData, minBounds, maxBounds)
    {
    }

    DX11Mesh::DX11Mesh(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords,
        const std::vector<float>& tangentData, const std::vector<Mat4>& boneMatrices, const std::vector<uint16_t>& boneIndices, const std::vector<float>& boneWeights, const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds) :
        mContext(context),
        mVertexBuffer(nullptr),
        mNormalBuffer(nullptr),
        mTangentBuffer(nullptr),
        mTexcoordBuffer(nullptr),
        mBoneMatrixBuffer(nullptr),
        mBoneIndexBuffer(nullptr),
        mBoneWeightBuffer(nullptr),
        mIndexBuffer(nullptr),
        mMeshID(gNextMeshID++),
        mBoneSRV(nullptr),
        mNumVertices(vertexData.size()),
        mNumIndices(indexData.size()),
        mHasBones(boneMatrices.size() && boneIndices.size() && boneWeights.size())
    {
        // TODO: split into more manageable code chunks
        // TODO: refactor slots etc

        // vertex buffer
        // use a temporary vector to merge vertices and AABB points
        std::vector<float> tempVertexData(vertexData);
        AppendAABBVertices(tempVertexData, minBounds, maxBounds);

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

        // bitangent/tangent buffer
        const size_t tangentDataSize = tangentData.size();
        if (tangentDataSize > 0)
        {
            ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
            bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
            bufferDescription.ByteWidth = tangentDataSize * sizeof(float);
            bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
            initData.pSysMem = &tangentData.at(0);
            DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mTangentBuffer));
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

        // bone matrices/indices/weights
        if (mHasBones)
        {
            // matrices
            ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
            bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
            bufferDescription.ByteWidth = boneMatrices.size() * sizeof(Mat4);
            bufferDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            bufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

            ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
            initData.pSysMem = &boneMatrices.at(0);
            DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mBoneMatrixBuffer));

            // indices
            ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
            bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
            bufferDescription.ByteWidth = boneIndices.size() * sizeof(uint16_t);
            bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
            initData.pSysMem = &boneIndices.at(0);
            DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mBoneIndexBuffer));

            // weights
            ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
            bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
            bufferDescription.ByteWidth = boneWeights.size() * sizeof(float);
            bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
            initData.pSysMem = &boneWeights.at(0);
            DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mBoneWeightBuffer));

            // bone matrix SRV
            DXCALL(device->CreateShaderResourceView(mBoneMatrixBuffer, nullptr, &mBoneSRV));
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
        mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_POSITIONS, 1, &mVertexBuffer.p, &gVertexSize, &gStaticOffset);
        if (mNormalBuffer)
            mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_NORMALS, 1, &mNormalBuffer.p, &gVertexSize, &gStaticOffset);
        if (mTexcoordBuffer)
            mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_TEXCOORDS, 1, &mTexcoordBuffer.p, &gTexcoordSize, &gStaticOffset);
        if (mTangentBuffer)
            mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_TANGENTS, 1, &mTangentBuffer.p, &gTangentSize, &gStaticOffset);
        if (mHasBones)
        {
            mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_BONE_INDICES, 1, &mBoneIndexBuffer.p, &gBoneIndexSize, &gStaticOffset);
            mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_BONE_WEIGHTS, 1, &mBoneWeightBuffer.p, &gBoneWeightSize, &gStaticOffset);
            // temp: register c0
            mContext->VSSetShaderResources(0, 1, &mBoneSRV.p);
        }

        mContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        mContext->DrawIndexed(mNumIndices, 0, 0);
    }

    void DX11Mesh::DrawPositions()
    {
        mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_POSITIONS, 1, &mVertexBuffer.p, &gVertexSize, &gStaticOffset);

        mContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        mContext->DrawIndexed(mNumIndices, 0, 0);
    }

    void DX11Mesh::DrawAABB()
    {
        // AABB indices are offset into the mesh index buffer
        const uint32_t offset = mNumVertices * sizeof(float);
        const uint32_t numAABBPoints = gAABBIndices.size();

        mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_POSITIONS, 1, &mVertexBuffer.p, &gVertexSize, &offset);
        mContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, mNumIndices * sizeof(uint16_t));
        mContext->DrawIndexed(numAABBPoints, 0, 0);
    }


    void AppendAABBVertices(std::vector<float>& vertexContainer, const Vec3& minBounds, const Vec3& maxBounds)
    {
        vertexContainer.push_back(minBounds.x); vertexContainer.push_back(minBounds.y); vertexContainer.push_back(minBounds.z);
        vertexContainer.push_back(minBounds.x); vertexContainer.push_back(maxBounds.y); vertexContainer.push_back(minBounds.z);
        vertexContainer.push_back(minBounds.x); vertexContainer.push_back(maxBounds.y); vertexContainer.push_back(maxBounds.z);
        vertexContainer.push_back(minBounds.x); vertexContainer.push_back(minBounds.y); vertexContainer.push_back(maxBounds.z);
        vertexContainer.push_back(maxBounds.x); vertexContainer.push_back(minBounds.y); vertexContainer.push_back(maxBounds.z);
        vertexContainer.push_back(maxBounds.x); vertexContainer.push_back(minBounds.y); vertexContainer.push_back(minBounds.z);
        vertexContainer.push_back(maxBounds.x); vertexContainer.push_back(maxBounds.y); vertexContainer.push_back(minBounds.z);
        vertexContainer.push_back(maxBounds.x); vertexContainer.push_back(maxBounds.y); vertexContainer.push_back(maxBounds.z);
    }
}

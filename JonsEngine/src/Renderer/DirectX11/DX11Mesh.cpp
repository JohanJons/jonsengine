#include "include/Renderer/DirectX11/DX11Mesh.h"

#include "include/Renderer/DirectX11/DX11VertexBufferSet.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Resources/Animation.h"

#include <array>

namespace JonsEngine
{
    const static uint32_t gPositionStride = sizeof(float) * 3;
    const static uint32_t gNormalStride = gPositionStride;
    const static uint32_t gTangentStride = gPositionStride * 2;
    const static uint32_t gTexcoordStride = sizeof(float) * 2;
    const static uint32_t gBoneWeightStride = sizeof(BoneWeight);
    const static uint32_t gStaticOffset = 0;

	// NOTE: not thread-safe, but wont be an issue
    static DX11MeshID gNextMeshID = 1;


	DX11Mesh::DX11Mesh(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<float>& vertexData, const std::vector<uint16_t>& indexData) :
		DX11Mesh(device, context, vertexData, std::vector<float>(), std::vector<float>(), std::vector<float>(), std::vector<BoneWeight>(), indexData)
	{
	}

    DX11Mesh::DX11Mesh(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords,
        const std::vector<float>& tangentData, const std::vector<uint16_t>& indexData) :
        DX11Mesh(device, context, vertexData, normalData, texCoords, tangentData, std::vector<BoneWeight>(), indexData)
    {
    }

	DX11Mesh::DX11Mesh(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords,
		const std::vector<float>& tangentData, const std::vector<BoneWeight>& boneWeights, const std::vector<uint16_t>& indexData) :
        mContext(context),
        mVertexBuffer(nullptr),
        mNormalBuffer(nullptr),
        mTangentBuffer(nullptr),
        mTexcoordBuffer(nullptr),
        mBoneWeightBuffer(nullptr),
        mIndexBuffer(nullptr),
        mMeshID(gNextMeshID++),

        mNumVertices(vertexData.size()),
        mNumIndices(indexData.size()),
        mHasBones(!boneWeights.empty())
    {
        // vertex buffer
        std::vector<float> tempVertexData(vertexData);

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

        // bone indices/weights
        if (mHasBones)
        {
			ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
			bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
			bufferDescription.ByteWidth = boneWeights.size() * sizeof(BoneWeight);
			bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
			initData.pSysMem = &boneWeights.at(0);
			DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mBoneWeightBuffer));
        }

        // index buffer
        // use a temporary vector to merge vertex indices and AABB indices
        std::vector<uint16_t> tempIndiceData(indexData);

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
        mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_POSITIONS, 1, &mVertexBuffer.p, &gPositionStride, &gStaticOffset);
        if (mNormalBuffer)
            mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_NORMALS, 1, &mNormalBuffer.p, &gNormalStride, &gStaticOffset);
        if (mTexcoordBuffer)
            mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_TEXCOORDS, 1, &mTexcoordBuffer.p, &gTexcoordStride, &gStaticOffset);
        if (mTangentBuffer)
            mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_TANGENTS, 1, &mTangentBuffer.p, &gTangentStride, &gStaticOffset);
        if (mHasBones)
            mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_BONE_WEIGHTS, 1, &mBoneWeightBuffer.p, &gBoneWeightStride, &gStaticOffset);

        mContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
        mContext->DrawIndexed(mNumIndices, 0, 0);
    }

    void DX11Mesh::DrawPositions()
    {
		DrawPositionsInstanced(1);
    }

	void DX11Mesh::DrawPositionsInstanced(const uint32_t numInstances)
	{
		mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_POSITIONS, 1, &mVertexBuffer.p, &gPositionStride, &gStaticOffset);
		if (mHasBones)
			mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_BONE_WEIGHTS, 1, &mBoneWeightBuffer.p, &gBoneWeightStride, &gStaticOffset);

		mContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		mContext->DrawIndexedInstanced(mNumIndices, numInstances, 0, 0, 0);
	}
}

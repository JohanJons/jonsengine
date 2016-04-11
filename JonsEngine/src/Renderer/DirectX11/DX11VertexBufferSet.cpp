#include "include/Renderer/DirectX11/DX11VertexBufferSet.hpp"

namespace JonsEngine
{
    DX11VertexBufferSet::~DX11VertexBufferSet()
    {
        for (ID3D11Buffer& buffer : mBuffers)
            buffer.Release();
    }


    void DX11VertexBufferSet::Bind()
    {
        const uint32_t startSlot = 0;
        const uint32_t offset = 0;
        ID3D11Buffer* buffers = &mBuffers.at(0);

        mContext->IASetVertexBuffers(startSlot, mNumBuffers, &buffers, &mStrides.at(0), &offset);
        /*mContext->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_POSITIONS, 1, &mVertexBuffer.p, &gVertexSize, &gStaticOffset);
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
        mContext->DrawIndexed(mNumIndices, 0, 0);*/
    }
}
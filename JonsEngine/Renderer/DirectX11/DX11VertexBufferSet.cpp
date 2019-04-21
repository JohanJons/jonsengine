#include "Renderer/DirectX11/DX11VertexBufferSet.hpp"

namespace JonsEngine
{
    DX11VertexBufferSet::~DX11VertexBufferSet()
    {
        for (ID3D11Buffer& buffer : mBuffers)
            buffer.Release();
    }


    void DX11VertexBufferSet::Bind(const uint32_t numBuffers)
    {
        const uint32_t startSlot = 0;
        const uint32_t offset = 0;
        ID3D11Buffer* buffers = &mBuffers.at(0);

        mContext->IASetVertexBuffers(startSlot, numBuffers, &buffers, &mStrides.at(0), &offset);
    }
}
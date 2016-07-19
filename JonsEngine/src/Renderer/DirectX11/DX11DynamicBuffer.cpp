#include "include/Renderer/DirectX11/DX11DynamicBuffer.h"

namespace JonsEngine
{
    DX11DynamicBuffer::DX11DynamicBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context) :
        mDevice(device),
        mContext(context),
        mBuffer(nullptr),
        mBufferSRV(nullptr),
        mBufferSizeInBytes(0)
    {
    }


    void DX11DynamicBuffer::Bind(const Shaderslot shaderSlot, const uint32_t bindSlot)
    {
        switch (shaderSlot)
        {
            case Shaderslot::Vertex:  mContext->VSSetShaderResources(bindSlot, 1, &mBufferSRV.p); break;
            case Shaderslot::Pixel:  mContext->VSSetShaderResources(bindSlot, 1, &mBufferSRV.p); break;
            default: break;
        }
    }
}
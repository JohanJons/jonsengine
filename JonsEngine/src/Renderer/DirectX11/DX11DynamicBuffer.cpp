#include "include/Renderer/DirectX11/DX11DynamicBuffer.h"

namespace JonsEngine
{
    DX11DynamicBuffer::DX11DynamicBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::size_t bufferSizeInBytes) :
        mContext(context),
        mBuffer(nullptr),
        mBufferSRV(nullptr),
        mBufferSizeInBytes(bufferSizeInBytes)
    {
        assert(bufferSizeInBytes > 0);

        D3D11_BUFFER_DESC bufferDescription;
        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = D3D11_USAGE_DEFAULT;
        bufferDescription.ByteWidth = bufferSizeInBytes;
        bufferDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        DXCALL(device->CreateBuffer(&bufferDescription, nullptr, &mBuffer));

        DXCALL(device->CreateShaderResourceView(mBuffer, nullptr, &mBufferSRV));
    }


    void DX11DynamicBuffer::Bind(const uint32_t bindSlot)
    {
        mContext->VSSetShaderResources(bindSlot, 1, &mBufferSRV.p);
    }
}
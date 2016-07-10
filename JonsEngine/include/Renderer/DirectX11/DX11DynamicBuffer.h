#pragma once

//#include "include/Renderer/DirectX11/DX11Buffer.hpp"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Platform/Directx11.h"

#include <vector>

namespace JonsEngine
{
    class DX11DynamicBuffer //: protected DX11Buffer<T, D3D11_USAGE_DYNAMIC>
    {
    public:
        DX11DynamicBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::size_t bufferSizeInBytes);

        void Bind(const uint32_t bindSlot);
        template <typename T> 
        void SetData(const std::vector<T>& data);


    private:
        ID3D11DeviceContextPtr mContext;
        ID3D11BufferPtr mBuffer;
        ID3D11ShaderResourceViewPtr mBufferSRV;
        std::size_t mBufferSizeInBytes;
    };


    template <typename T>
    void DX11DynamicBuffer::SetData(const std::vector<T>& data)
    {
        const uint32_t dataSize = data.size() * sizeof(T);
        if (dataSize > mBufferSizeInBytes)
            // recreate buffer...

        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

        DXCALL(mContext->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
        std::memcpy(mappedResource.pData, &data.front(), dataSize);
        mContext->Unmap(mBuffer, 0);
    }
}
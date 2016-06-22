#pragma once

#include "include/Core/Types.h"
#include "include/Core/Platform/Directx11.h"
#include "include/Renderer/DirectX11/DX11Utils.h"

#include <vector>

namespace JonsEngine
{
    // TODO: extend with support for more bindFlags, and variadic template for multiple bindFlags
    template <typename T, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlags>
    class DX11Buffer
    {
    public:
        static_assert(bindFlags == D3D11_BIND_VERTEX_BUFFER || bindFlags == D3D11_BIND_INDEX_BUFFER, "DX11Buffer: Only supported bindFlags: D3D11_BIND_VERTEX_BUFFER and D3D11_BIND_INDEX_BUFFER");

        DX11Buffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const BindFlag bindFlag, const std::vector<T>& data);
        DX11Buffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const BindFlag bindFlag, const std::size_t bufferSizeInBytes);

        void Bind();
        
        
    private:
        D3D11_BUFFER_DESC GetBufferDesc(const BindFlag bindFlag, const std::size_t bufferSizeInBytes);
    
    
        ID3D11BufferPtr mBuffer;
        ID3D11DeviceContextPtr mContext;
    };
    
    
    template <typename T, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlags>
    DX11Buffer<T, usage, bindFlags>::DX11Buffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const BindFlag bindFlag, const std::vector<T>& data) :
        mBuffer(nullptr),
        mContext(context)
    {
        assert(!data.empty());
        
        const std::size_t dataSizeInBytes = data.size() * sizeof(T);
        D3D11_BUFFER_DESC bufferDescription = GetBufferDesc(bindFlag, dataSizeInBytes);

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = &data.at(0);
        DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mBuffer));
    }
    
    template <typename T, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlags>
    DX11Buffer<T, usage, bindFlags>::DX11Buffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const BindFlag bindFlag, const std::size_t bufferSizeInBytes) :
        mBuffer(nullptr),
        mContext(context)
    {
        static_assert(usage == D3D11_USAGE_IMMUTABLE, "DX11Buffer::DX11Buffer(): For IMMMUTABLE buffers initial data must be provided.");
        assert(bufferSizeInBytes > 0);
            
        D3D11_BUFFER_DESC bufferDescription = GetBufferDesc(bindFlag, dataSizeInBytes);
        
        DXCALL(device->CreateBuffer(&bufferDescription, nullptr, &mBuffer));
    }
    
    
    // work around for partial template specialization unavailable
    template<typename T, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlags>
    struct _DX11Buffer;
    template<typename T, D3D11_USAGE usage, D3D11_BIND_VERTEX_BUFFER>
    struct _DX11Buffer
    {
        static void Bind(ID3D11DeviceContextPtr context, const uint32_t startSlot, const uint32_t position, const uint32_t stride)
        {
            context->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_POSITIONS, 1, &mVertexBuffer.p, &gPositionStride, &gStaticOffset);
        }
    }
    template<typename T, D3D11_USAGE usage, D3D11_BIND_INDEX_BUFFER>
    struct _DX11Buffer
    {
        static void Bind(ID3D11DeviceContextPtr context, const uint32_t startSlot, const uint32_t position, const uint32_t stride)
        {
            context->IASetVertexBuffers(VertexBufferSlot::VERTEX_BUFFER_SLOT_POSITIONS, 1, &mVertexBuffer.p, &gPositionStride, &gStaticOffset);
        }
    }
    
    template <typename T, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlags>
    void DX11Buffer<T, usage, bindFlags>::Bind(const uint32_t startSlot, const uint32_t position, const uint32_t stride)
    {
        return _DX11Buffer<T, usage, bindFlags>::Bind(mContext, const uint32_t position, const uint32_t stride);
    }
    
    
    template <typename T, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlags>
    D3D11_BUFFER_DESC DX11Buffer<T, usage>::GetBufferDesc(const BindFlag bindFlag, const std::size_t bufferSizeInBytes)
    {
        D3D11_BUFFER_DESC bufferDescription;
        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = usage;
        bufferDescription.ByteWidth = bufferSizeInBytes;
        bufferDescription.BindFlags = bindFlag == BindFlag::VertexBuffer ? D3D11_BIND_VERTEX_BUFFER : D3D11_BIND_INDEX_BUFFER;
        
        return bufferDescription;
    }
}
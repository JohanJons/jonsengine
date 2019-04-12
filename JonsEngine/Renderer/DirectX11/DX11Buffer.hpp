#pragma once

#include "Types.h"
#include "Directx11.h"
#include "DX11Utils.h"

#include <vector>

namespace JonsEngine
{
    template <typename T, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlag>
    class DX11Buffer
    {
    public:
        //static_assert(bindFlags == D3D11_BIND_VERTEX_BUFFER || bindFlags == D3D11_BIND_INDEX_BUFFER, "DX11Buffer: Only supported bindFlags: D3D11_BIND_VERTEX_BUFFER and D3D11_BIND_INDEX_BUFFER");

        DX11Buffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<T>& data);
        DX11Buffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::size_t bufferSizeInBytes);

        
    protected:
        D3D11_BUFFER_DESC GetBufferDesc(const std::size_t bufferSizeInBytes);
    
    
        ID3D11DeviceContextPtr mContext;
        ID3D11BufferPtr mBuffer;
    };
    
    
    template <typename T, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlag>
    DX11Buffer<T, usage, bindFlag>::DX11Buffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<T>& data) :
        mContext(context),
        mBuffer(nullptr)
    {
        assert(!data.empty());
        
        const std::size_t dataSizeInBytes = data.size() * sizeof(T);
        D3D11_BUFFER_DESC bufferDescription = GetBufferDesc(bindFlag, dataSizeInBytes);

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = &data.at(0);
        DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mBuffer));
    }
    
    template <typename T, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlag>
    DX11Buffer<T, usage, bindFlag>::DX11Buffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::size_t bufferSizeInBytes) :
        mBuffer(nullptr),
        mContext(context)
    {
        static_assert(usage == D3D11_USAGE_IMMUTABLE, "DX11Buffer::DX11Buffer(): For IMMMUTABLE buffers initial data must be provided.");
        assert(bufferSizeInBytes > 0);
            
        D3D11_BUFFER_DESC bufferDescription = GetBufferDesc(bindFlag, dataSizeInBytes);
        
        DXCALL(device->CreateBuffer(&bufferDescription, nullptr, &mBuffer));
    }
    
    
    template <typename T, D3D11_USAGE usage, D3D11_BIND_FLAG bindFlag>
    D3D11_BUFFER_DESC DX11Buffer<T, usage, bindFlag>::GetBufferDesc(const std::size_t bufferSizeInBytes)
    {
        D3D11_BUFFER_DESC bufferDescription;
        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = usage;
        bufferDescription.ByteWidth = bufferSizeInBytes;
        bufferDescription.BindFlags = bindFlag;
        
        return bufferDescription;
    }
}
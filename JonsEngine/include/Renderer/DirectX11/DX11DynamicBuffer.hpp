#pragma once

#include "include/Renderer/DirectX11/DX11Buffer.h"
#include "include/Core/Platform/Directx11.h"

namespace JonsEngine
{
    template <typename T>
    class DX11DynamicBuffer : public DX11Buffer<T, D3D11_USAGE_DYNAMIC>
    {
    public:
        DX11DynamicBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const BindFlag bindFlag, const std::size_t bufferSizeInBytes);
    };
    
    
    template <typename T>
    DX11DynamicBuffer<T>::DX11DynamicBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const BindFlag bindFlag, const std::size_t bufferSizeInBytes) : DX11Buffer<T>(device, context, bindFlag, bufferSizeInBytes)
    {
    }
    
    
}
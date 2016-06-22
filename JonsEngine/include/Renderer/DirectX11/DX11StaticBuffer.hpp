#pragma once

#include "include/Renderer/DirectX11/DX11Buffer.h"
#include "include/Core/Platform/Directx11.h"

#include <vector>

namespace JonsEngine
{
    template <typename T>
    class DX11StaticBuffer : public DX11Buffer<T, D3D11_USAGE_IMMUTABLE>
    {
    public:
        DX11StaticBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const BindFlag bindFlag, const std::vector<T>& data);
    };
    
    
    template <typename T>
    DX11StaticBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const BindFlag bindFlag, const std::vector<T>& data) : DX11Buffer<T>(device, context, bindFlag, data)
    {
    }
}
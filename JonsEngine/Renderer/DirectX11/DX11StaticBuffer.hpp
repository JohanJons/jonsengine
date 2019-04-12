#pragma once

#include "DX11Buffer.hpp"
#include "Platform.h"

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
    DX11StaticBuffer<T>::DX11StaticBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const BindFlag bindFlag, const std::vector<T>& data) : DX11Buffer<T>(device, context, bindFlag, data)
    {
    }
}
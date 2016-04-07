#pragma once

#include "include/Core/Types.h"
#include "include/Core/Platform/Directx11.h"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Buffer.hpp"

#include <vector>

namespace JonsEngine
{
    template <typename T>
    class DX11VertexBuffer
    {
    public:
        DX11VertexBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const std::vector<T>& data) : 
        {
        }
        
        ~DX11VertexBuffer()
        {
        }


    private:
        DX11Buffer<T> mVertexBuffer;
        ID3D11DeviceContextPtr mContext;
    };
}

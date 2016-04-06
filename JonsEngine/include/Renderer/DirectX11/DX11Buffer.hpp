#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"
#include "include/Core/Platform/Directx11.h"

#include <vector>

namespace JonsEngine
{
    template <typename T>
    class DX11Buffer
    {
    public:
        enum class BindFlag
        {
            VertexBuffer,
            IndexBuffer
        };
    
        DX11Buffer(ID3D11DevicePtr device, const BindFlag bindFlag, const std::vector<T>& data);
        ~DX11Buffer();

        
    private:
        ID3D11BufferPtr mBuffer;
    };
}

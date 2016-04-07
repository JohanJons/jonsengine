#pragma once

#include "include/Core/Types.h"
#include "include/Core/Platform/Directx11.h"
#include "include/Renderer/DirectX11/DX11Utils.h"

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
    
        DX11Buffer(ID3D11DevicePtr device, const BindFlag bindFlag, const std::vector<T>& data) : mBuffer(nullptr)
        {
            D3D11_BUFFER_DESC bufferDescription;
            ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
            bufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
            bufferDescription.ByteWidth = data.size() * sizeof(T);
            bufferDescription.BindFlags = bindFlag == BindFlag::VertexBuffer ? D3D11_BIND_VERTEX_BUFFER : D3D11_BIND_INDEX_BUFFER;

            D3D11_SUBRESOURCE_DATA initData;
            ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
            initData.pSysMem = &data.at(0);
            DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mBuffer));
        }
        
        ~DX11Buffer()
        {
        }

        
    private:
        ID3D11BufferPtr mBuffer;
    };
}

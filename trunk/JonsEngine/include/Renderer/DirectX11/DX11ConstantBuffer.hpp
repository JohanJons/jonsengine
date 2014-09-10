#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/Shaders/Constants.h"

#include <d3d11.h>
#include <cstring>

namespace JonsEngine
{
    template <typename ContentType>
    class DX11ConstantBuffer
    {
    public:
        enum CONSTANT_BUFFER_SLOT
        {
            CONSTANT_BUFFER_SLOT_VERTEX = CBUFFER_SLOT_VERTEX,
            CONSTANT_BUFFER_SLOT_PIXEL = CBUFFER_SLOT_PIXEL
        };

        DX11ConstantBuffer(ID3D11DevicePtr device, const CONSTANT_BUFFER_SLOT cbufferSlot) : mConstantBuffer(nullptr), mConstantBufferSlot(cbufferSlot)
        {
            D3D11_BUFFER_DESC bufferDescription;
            ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
            bufferDescription.ByteWidth = sizeof(ContentType);
            bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
            bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            DXCALL(device->CreateBuffer(&bufferDescription, NULL, &mConstantBuffer));
        }

        ~DX11ConstantBuffer()
        {
        }


        void SetData(const ContentType& content, ID3D11DeviceContextPtr context)
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

            DXCALL(context->Map(mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
            std::memcpy(mappedResource.pData, &content, sizeof(ContentType));
            context->Unmap(mConstantBuffer, 0);

            context->VSSetConstantBuffers(mConstantBufferSlot, 1, &mConstantBuffer.p);
            context->PSSetConstantBuffers(mConstantBufferSlot, 1, &mConstantBuffer.p);
        }


    private:
        ID3D11BufferPtr mConstantBuffer;
        const CONSTANT_BUFFER_SLOT mConstantBufferSlot;
    };
}

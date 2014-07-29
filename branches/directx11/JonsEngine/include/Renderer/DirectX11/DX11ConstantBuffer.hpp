#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"

#include <d3d11.h>
#include <cstring>

namespace JonsEngine
{
    template <typename ContentType>
    class DX11ConstantBuffer
    {
    public:
        DX11ConstantBuffer(ID3D11Device* device) : mConstantBuffer(nullptr)
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
            mConstantBuffer->Release();
        }


        void SetData(const ContentType& content, ID3D11DeviceContext* context, uint32_t bufferSlot)
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

            DXCALL(context->Map(mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
            std::memcpy(mappedResource.pData, &content, sizeof(ContentType));
            context->Unmap(mConstantBuffer, 0);

            context->VSSetConstantBuffers(bufferSlot, 1, &mConstantBuffer);
            context->PSSetConstantBuffers(bufferSlot, 1, &mConstantBuffer);
        }


    private:
        ID3D11Buffer* mConstantBuffer;
    };
}
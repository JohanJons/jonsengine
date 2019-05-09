#pragma once

#include "Renderer/DirectX11/DX11Utils.h"
#include "Shaders/Constants.hlsl"
#include "Core/Platform.h"

namespace JonsEngine
{
    template <typename ContentType>
    class DX11ConstantBuffer
    {
    public:
        enum CONSTANT_BUFFER_SLOT
        {
            CONSTANT_BUFFER_SLOT_VERTEX = CBUFFER_SLOT_VERTEX,
            CONSTANT_BUFFER_SLOT_PIXEL = CBUFFER_SLOT_PIXEL,
            CONSTANT_BUFFER_SLOT_COMPUTE = CBUFFER_SLOT_COMPUTE,
			CONSTANT_BUFFER_SLOT_DOMAIN = CBUFFER_SLOT_DOMAIN,
			CONSTANT_BUFFER_SLOT_HULL = CBUFFER_SLOT_HULL,
			CONSTANT_BUFFER_SLOT_PER_FRAME = CBUFFER_SLOT_PER_FRAME,
			CONSTANT_BUFFER_SLOT_EXTRA = CBUFFER_SLOT_EXTRA,
        };

        DX11ConstantBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const CONSTANT_BUFFER_SLOT cbufferSlot) :
            mContext(context), mConstantBuffer(nullptr), mConstantBufferSlot(cbufferSlot)
        {
            D3D11_BUFFER_DESC bufferDescription;
            ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
            bufferDescription.ByteWidth = static_cast<uint32_t>( sizeof(ContentType) );
            bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
            bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            DXCALL(device->CreateBuffer(&bufferDescription, NULL, &mConstantBuffer));
        }


        void SetData(const ContentType& content)
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

            DXCALL(mContext->Map(mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
            std::memcpy(mappedResource.pData, &content, sizeof(ContentType));
            mContext->Unmap(mConstantBuffer, 0);
        }

		void Bind()
		{
			// TODO: split per type?
			mContext->VSSetConstantBuffers( mConstantBufferSlot, 1, &mConstantBuffer.p );
			mContext->PSSetConstantBuffers( mConstantBufferSlot, 1, &mConstantBuffer.p );
			mContext->CSSetConstantBuffers( mConstantBufferSlot, 1, &mConstantBuffer.p );
			mContext->DSSetConstantBuffers( mConstantBufferSlot, 1, &mConstantBuffer.p );
			mContext->HSSetConstantBuffers( mConstantBufferSlot, 1, &mConstantBuffer.p );
		}


    private:
        ID3D11DeviceContextPtr mContext;
        ID3D11BufferPtr mConstantBuffer;
        const CONSTANT_BUFFER_SLOT mConstantBufferSlot;
    };
}

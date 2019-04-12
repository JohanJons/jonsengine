#pragma once

#include "DX11Utils.h"
#include "Directx11.h"

#include <vector>

namespace JonsEngine
{
    class DX11CPUDynamicBuffer //: protected DX11Buffer<T, D3D11_USAGE_DYNAMIC>
    {
    public:
        enum class Shaderslot
        {
            Vertex,
            Pixel
        };

        DX11CPUDynamicBuffer(ID3D11DevicePtr device, ID3D11DeviceContextPtr context);

        void Bind(Shaderslot shaderSlot, uint32_t bindSlot);

        template <typename T> 
        void SetData( const std::vector<T>& data );


    private:
		template <typename T>
		void SetData( const T* dataptr, std::size_t totalSizeInBytes );
        template <typename T>
        void ResizeBuffer(const T* data, std::size_t newSizeInBytes);
        template <typename T>
        void BufferData(const T* data, std::size_t newSizeInBytes);


        ID3D11DevicePtr mDevice;
        ID3D11DeviceContextPtr mContext;
        ID3D11BufferPtr mBuffer;
        ID3D11ShaderResourceViewPtr mBufferSRV;
        std::size_t mBufferSizeInBytes;
    };


    template <typename T>
    void DX11CPUDynamicBuffer::SetData(const std::vector<T>& data)
    {
        const std::size_t dataSize = data.size() * sizeof(T);
        const T* dataBegin = &data.front();
		
		SetData(dataBegin, dataSize);
	}

	template <typename T>
	void DX11CPUDynamicBuffer::SetData( const T* dataptr, const std::size_t totalSizeInBytes )
	{
		assert( dataptr );

		if ( totalSizeInBytes > mBufferSizeInBytes )
			ResizeBuffer( dataptr, totalSizeInBytes );
		else
			BufferData( dataptr, totalSizeInBytes );

		mBufferSizeInBytes = totalSizeInBytes;
	}


    template <typename T>
    void DX11CPUDynamicBuffer::ResizeBuffer(const T* data, const std::size_t newSizeInBytes)
    {
		mBuffer.Release();
		mBufferSRV.Release();

        D3D11_SUBRESOURCE_DATA initialData;
        ZeroMemory(&initialData, sizeof(D3D11_SUBRESOURCE_DATA));
        initialData.pSysMem = data;
        initialData.SysMemPitch = newSizeInBytes;

        D3D11_BUFFER_DESC bufferDescription;
        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
        bufferDescription.ByteWidth = newSizeInBytes;
		bufferDescription.StructureByteStride = sizeof(T);
        bufferDescription.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDescription.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        DXCALL(mDevice->CreateBuffer(&bufferDescription, &initialData, &mBuffer));

        DXCALL(mDevice->CreateShaderResourceView(mBuffer, nullptr, &mBufferSRV));
    }

    template <typename T>
    void DX11CPUDynamicBuffer::BufferData(const T* data, const std::size_t newSizeInBytes)
    {
        D3D11_MAPPED_SUBRESOURCE mappedResource;
        ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

        DXCALL(mContext->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
        std::memcpy(mappedResource.pData, data, newSizeInBytes);
        mContext->Unmap(mBuffer, 0);
    }
}
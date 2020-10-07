#include "Renderer/DirectX11/DX11DepthBufferReadback.h"

#include "Compiled/DepthReadback.h"

namespace JonsEngine
{
	DX11DepthBufferReadback::DX11DepthBufferReadback(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, ID3D11Texture2DPtr depthBuffer) :
		mContext(context),
		mCSBuffer(nullptr),
		mStagingBuffer(nullptr),
		mCSUAV(nullptr),
		mReadbackShader(nullptr),
		mDepthbuffer(depthBuffer),
		mCBuffer(device, context, DX11ConstantBuffer<CBuffer>::CONSTANT_BUFFER_SLOT_COMPUTE)
	{
		D3D11_BUFFER_DESC bufferDesc;
		ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		bufferDesc.ByteWidth = static_cast<uint32_t>( sizeof(float) );
		device->CreateBuffer(&bufferDesc, nullptr, &mCSBuffer);

		bufferDesc.Usage = D3D11_USAGE_STAGING;
		bufferDesc.BindFlags = 0;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		device->CreateBuffer(&bufferDesc, nullptr, &mStagingBuffer);

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
		uavDesc.Buffer.NumElements = 1;
		uavDesc.Format = DXGI_FORMAT_R32_FLOAT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		device->CreateUnorderedAccessView(mCSBuffer, &uavDesc, &mCSUAV);

		device->CreateComputeShader(gDepthReadback, sizeof(gDepthReadback), nullptr, &mReadbackShader);
	}


	float DX11DepthBufferReadback::GetDepthValue(const WindowPosition& position)
	{
		RenderDepth(position);

		return ReadDepthValue();
	}


	void DX11DepthBufferReadback::RenderDepth(const WindowPosition& position)
	{
		D3D11_TEXTURE2D_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(D3D11_TEXTURE2D_DESC));
		mDepthbuffer->GetDesc(&depthDesc);

		mContext->CSSetShader(mReadbackShader, nullptr, 0);
		mContext->CSSetUnorderedAccessViews(UAV_SLOT_0, 1, &mCSUAV.p, nullptr);
		mCBuffer.SetData({ { position.x, position.y } });
		mCBuffer.Bind();

		mContext->Dispatch(1, 1, 1);
	}

	float DX11DepthBufferReadback::ReadDepthValue()
	{
		mContext->CopyResource(mStagingBuffer, mCSBuffer);

		D3D11_MAPPED_SUBRESOURCE mappedRes;
		mContext->Map(mStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedRes);
		float depth = *static_cast<float*>(mappedRes.pData);
		mContext->Unmap(mStagingBuffer, 0);

		return depth;
	}
}
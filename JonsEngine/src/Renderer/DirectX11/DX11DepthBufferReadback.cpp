#include "include/Renderer/DirectX11/DX11DepthBufferReadback.h"

namespace JonsEngine
{
	DX11DepthBufferReadback::DX11DepthBufferReadback(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, ID3D11Texture2DPtr depthBuffer) :
		mContext(context),
		mCSOutputTexture(nullptr),
		mDepthbuffer(depthBuffer),
		mStaging1x1(nullptr),
		mCBuffer(device, context, DX11ConstantBuffer<CBuffer>::CONSTANT_BUFFER_SLOT_COMPUTE)
	{
		D3D11_TEXTURE2D_DESC outputTextureDesc;
		ZeroMemory(&outputTextureDesc, sizeof(outputTextureDesc));
		outputTextureDesc.ArraySize = 1;
		outputTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
		outputTextureDesc.Width = 1;
		outputTextureDesc.Height = 1;
		outputTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		outputTextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
		outputTextureDesc.MipLevels = 1;
		outputTextureDesc.SampleDesc.Count = 1;
		outputTextureDesc.SampleDesc.Quality = 0;
		outputTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		device->CreateTexture2D(&outputTextureDesc, NULL, &mCSOutputTexture);

		D3D11_TEXTURE2D_DESC depthTextureDesc;
		ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
		depthBuffer->GetDesc(&depthTextureDesc);

		depthTextureDesc.Usage = D3D11_USAGE_STAGING;
		depthTextureDesc.BindFlags = 0;
		depthTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		device->CreateTexture2D(&depthTextureDesc, NULL, &mStaging1x1);
	}


	float DX11DepthBufferReadback::GetDepthValue(const WindowPosition& position) const
	{
		D3D11_BOX box;
		ZeroMemory(&box, sizeof(box));
		box.left = position.x;
		box.right = box.left + 1;
		box.top = position.y;
		box.bottom = box.top + 1;
		box.front = 0;
		box.back = 1;

		mContext->CopySubresourceRegion(mStaging1x1, 0, 0, 0, 0, mDepthbuffer, 0, &box);

		D3D11_MAPPED_SUBRESOURCE mappedRes;
		mContext->Map(mStaging1x1, 0, D3D11_MAP_READ, 0, &mappedRes);
		float depth = *static_cast<float*>(mappedRes.pData);
		mContext->Unmap(mStaging1x1, 0);

		return depth;
	}
}
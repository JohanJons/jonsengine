#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Window/Types.h"

namespace JonsEngine
{
	class DX11DepthBufferReadback
	{
	public:
		DX11DepthBufferReadback(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, ID3D11Texture2DPtr depthBuffer);

		float GetDepthValue(const WindowPosition& position) const;


	private:
		struct CBuffer
		{
			float mUVCoords;
		};

		ID3D11DeviceContextPtr mContext;
		ID3D11Texture2DPtr mCSOutputTexture;
		ID3D11Texture2DPtr mStaging1x1;
		ID3D11Texture2DPtr mDepthbuffer;
		DX11ConstantBuffer<CBuffer> mCBuffer;
	};
}
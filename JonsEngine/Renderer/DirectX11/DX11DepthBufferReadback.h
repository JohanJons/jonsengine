#pragma once

#include "Renderer/DirectX11/DX11Utils.h"
#include "Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "Core/Types.h"

namespace JonsEngine
{
	class DX11DepthBufferReadback
	{
	public:
		DX11DepthBufferReadback(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, ID3D11Texture2DPtr depthBuffer);

		float GetDepthValue(const WindowPosition& position);


	private:
		struct CBuffer
		{
			uVec2 mUVCoords;
			float __padding[2];
		};

		void RenderDepth(const WindowPosition& position);
		float ReadDepthValue();


		ID3D11DeviceContextPtr mContext;
		ID3D11BufferPtr mCSBuffer;
		ID3D11BufferPtr mStagingBuffer;
		ID3D11UnorderedAccessViewPtr mCSUAV;
		ID3D11ComputeShaderPtr mReadbackShader;
		ID3D11Texture2DPtr mDepthbuffer;
		DX11ConstantBuffer<CBuffer> mCBuffer;
	};
}
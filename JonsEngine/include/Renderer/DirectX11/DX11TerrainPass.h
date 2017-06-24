#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/RenderSettings.h"

namespace JonsEngine
{
	class DX11Mesh;

	class DX11TerrainPass
	{
	public:
		DX11TerrainPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const RenderSettings::Tesselation& tessData);
		~DX11TerrainPass();

		void Render(DX11Mesh& mesh, const Mat4& worldTransform);
		void BindForRendering();


	private:
		struct TerrainCBuffer
		{
			Mat4 mWorldTransform;
			float mMinDistance;
			float mMaxDistance;
			float mMinFactor;
			float mMaxFactor;
		};

		ID3D11DeviceContextPtr mContext;
		ID3D11PixelShaderPtr mPixelShader;
		DX11ConstantBuffer<TerrainCBuffer> mCBuffer;
		ID3D11VertexShaderPtr mVertexShader;
		ID3D11HullShaderPtr mHullShader;
		ID3D11DomainShaderPtr mDomainShader;

		RenderSettings::Tesselation mTessData;
	};
}
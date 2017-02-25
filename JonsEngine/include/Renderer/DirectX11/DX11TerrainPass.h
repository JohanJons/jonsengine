#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/RenderSettings.h"

namespace JonsEngine
{
	class DX11TerrainPass
	{
	public:
		DX11TerrainPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const RenderSettings::Tesselation& tessData);
		~DX11TerrainPass();

		void Render();


	private:
		ID3D11DeviceContextPtr mContext;
		ID3D11PixelShaderPtr mPixelShader;
		RenderSettings::Tesselation mTessData;
	};
}
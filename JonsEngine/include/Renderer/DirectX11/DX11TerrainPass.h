#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
	class DX11TerrainPass
	{
	public:
		DX11TerrainPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context);
		~DX11TerrainPass();

		void Render();


	private:
		ID3D11DeviceContextPtr mContext;
		ID3D11PixelShaderPtr mPixelShader;
	};
}
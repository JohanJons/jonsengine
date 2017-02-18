#include "include/Renderer/DirectX11/DX11TerrainPass.h"

namespace JonsEngine
{
	DX11TerrainPass::DX11TerrainPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context) :
		mContext(nullptr),
		mPixelShader(nullptr)
	{
	}

	DX11TerrainPass::~DX11TerrainPass()
	{
	}


	void DX11TerrainPass::Render()
	{

	}
}
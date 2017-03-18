#include "include/Renderer/DirectX11/DX11TerrainPass.h"

#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TerrainVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TerrainHull.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TerrainDomain.h"

namespace JonsEngine
{
	DX11TerrainPass::DX11TerrainPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, const RenderSettings::Tesselation& tessData) :
		mContext(nullptr),
		mPixelShader(nullptr),
		mCBuffer(device, context, mCBuffer.CONSTANT_BUFFER_SLOT_VERTEX),
		mTessData(tessData),
		mVertexShader(nullptr),
		mHullShader(nullptr),
		mDomainShader(nullptr)
	{
		DXCALL(device->CreateVertexShader(gTerrainVertexShader, sizeof(gTerrainVertexShader), nullptr, &mVertexShader));
		DXCALL(device->CreateHullShader(gTerrainHullShader, sizeof(gTerrainHullShader), nullptr, &mHullShader));
		DXCALL(device->CreateDomainShader(gTerrainDomainShader, sizeof(gTerrainDomainShader), nullptr, &mDomainShader));
	}

	DX11TerrainPass::~DX11TerrainPass()
	{
	}


	void DX11TerrainPass::Render(DX11Mesh& mesh)
	{
		mesh.Draw();
	}

	void DX11TerrainPass::BindForRendering()
	{
		mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		mCBuffer.SetData({ mTessData.mMinDistance, mTessData.mMaxDistance, mTessData.mMinFactor, mTessData.mMaxFactor, });
	}
}
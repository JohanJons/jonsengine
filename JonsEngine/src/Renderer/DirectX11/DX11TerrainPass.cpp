#include "include/Renderer/DirectX11/DX11TerrainPass.h"

#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TerrainVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TerrainHull.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TerrainDomain.h"

namespace JonsEngine
{
	const std::vector<float> gQuadVertices{
		-1.0f, 0.0f, -1.0f,
		-1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, -1.0f
	};

	const Vec3 gAABBMax(1.0f, 0.0f, 1.0f), gAABBMin(-1.0f, 0.0f, -1.0f);

	const std::vector<uint16_t> gQuadIndices{ 0, 2, 1, 3, 2, 0 };


	DX11TerrainPass::DX11TerrainPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformer, const RenderSettings::Tesselation& tessData) :
		mContext(context),
		mPixelShader(nullptr),
		mVertexShader(nullptr),
		mHullShader(nullptr),
		mDomainShader(nullptr),

		mCBuffer(device, context, mCBuffer.CONSTANT_BUFFER_SLOT_VERTEX),
		mQuadMesh(device, context, gQuadVertices, gQuadIndices, gAABBMin, gAABBMax),
		mVertexTransformer(vertexTransformer),

		mTessData(tessData)
	{
		DXCALL(device->CreateVertexShader(gTerrainVertexShader, sizeof(gTerrainVertexShader), nullptr, &mVertexShader));
		DXCALL(device->CreateHullShader(gTerrainHullShader, sizeof(gTerrainHullShader), nullptr, &mHullShader));
		DXCALL(device->CreateDomainShader(gTerrainDomainShader, sizeof(gTerrainDomainShader), nullptr, &mDomainShader));
	}

	DX11TerrainPass::~DX11TerrainPass()
	{
	}


	void DX11TerrainPass::Render(DX11Texture& texture, const Mat4& worldTransform, const Mat4& viewProjection)
	{
		texture.BindAsShaderResource(SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA);
		mVertexTransformer.RenderStaticMesh(mQuadMesh, worldTransform * viewProjection);

		//mCBuffer.SetData({ worldTransform, 0.0f, 0.0f, 0.0f, 0.0f });
		//mesh.Draw();
	}

	void DX11TerrainPass::BindForRendering()
	{
		mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		mCBuffer.SetData({ Mat4(1.0f), mTessData.mMinDistance, mTessData.mMaxDistance, mTessData.mMinFactor, mTessData.mMaxFactor });
	}
}
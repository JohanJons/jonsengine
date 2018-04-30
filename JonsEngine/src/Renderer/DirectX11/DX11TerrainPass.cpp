#include "include/Renderer/DirectX11/DX11TerrainPass.h"

#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TerrainVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TerrainHull.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TerrainDomain.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/TerrainPixel.h"
#include "include/RenderQueue/RenderableTerrain.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Math/AABB.h"

namespace JonsEngine
{
	const std::vector<float> gQuadVertices{
		-1.0f, 0.0f, -1.0f,
		1.0f, 0.0f, -1.0f,
		1.0f, 0.0f, 1.0f,
		-1.0f, 0.0f, 1.0f
	};

	const std::vector<float> gQuadNormals{
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	const std::vector<uint16_t> gQuadIndices{ 0, 1, 2, 3 };

	enum VSInputLayout
	{
		POSITION = 0,
		NORMAL,
		NUM_INPUT_LAYOUTS
	};


	DX11TerrainPass::DX11TerrainPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformer, const IDMap<DX11Texture>& textureMap, const RenderSettings::Tesselation& tessData) :
		mContext(context),

		mCBuffer(device, context, mCBuffer.CONSTANT_BUFFER_SLOT_DOMAIN),
		mPerTerrainCBuffer(device, context, mPerTerrainCBuffer.CONSTANT_BUFFER_SLOT_EXTRA),
		mTransformsBuffer(device, context),
		mQuadMesh(device, context, gQuadVertices, gQuadNormals, std::vector<float>(), std::vector<float>(), gQuadIndices, AABB::gUnitQuadAABB.Min(), AABB::gUnitQuadAABB.Max()),
		mVertexTransformer(vertexTransformer),
		mTextureMap(textureMap),

		mTessData(tessData)
	{
		// input layout
		// TODO: generalize into one class
		D3D11_INPUT_ELEMENT_DESC inputDescription[NUM_INPUT_LAYOUTS];
		ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC) * NUM_INPUT_LAYOUTS);

		inputDescription[VSInputLayout::POSITION].SemanticName = "POSITION";
		inputDescription[VSInputLayout::POSITION].SemanticIndex = 0;
		inputDescription[VSInputLayout::POSITION].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputDescription[VSInputLayout::POSITION].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_POSITIONS;
		inputDescription[VSInputLayout::POSITION].AlignedByteOffset = 0;
		inputDescription[VSInputLayout::POSITION].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDescription[VSInputLayout::POSITION].InstanceDataStepRate = 0;

		inputDescription[VSInputLayout::NORMAL].SemanticName = "NORMAL";
		inputDescription[VSInputLayout::NORMAL].SemanticIndex = 0;
		inputDescription[VSInputLayout::NORMAL].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputDescription[VSInputLayout::NORMAL].InputSlot = DX11Mesh::VERTEX_BUFFER_SLOT_NORMALS;
		inputDescription[VSInputLayout::NORMAL].AlignedByteOffset = 0;
		inputDescription[VSInputLayout::NORMAL].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputDescription[VSInputLayout::NORMAL].InstanceDataStepRate = 0;
		DXCALL(device->CreateInputLayout(inputDescription, VSInputLayout::NUM_INPUT_LAYOUTS, gTerrainVertexShader, sizeof(gTerrainVertexShader), &mLayout));

		// shaders
		DXCALL(device->CreateVertexShader(gTerrainVertexShader, sizeof(gTerrainVertexShader), nullptr, &mVertexShader));
		DXCALL(device->CreateHullShader(gTerrainHullShader, sizeof(gTerrainHullShader), nullptr, &mHullShader));
		DXCALL(device->CreateDomainShader(gTerrainDomainShader, sizeof(gTerrainDomainShader), nullptr, &mDomainShader));
		DXCALL(device->CreatePixelShader(gTerrainPixelShader, sizeof(gTerrainPixelShader), nullptr, &mPixelShader));

		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise = true;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.AntialiasedLineEnable = false;
		DXCALL(device->CreateRasterizerState(&rasterizerDesc, &mDebugRasterizer));
	}

	void DX11TerrainPass::Render(const RenderableTerrains& terrains)
	{
		if ( !terrains.GetNumTerrains() )
			return;

		BindForRendering();

		ID3D11RasterizerStatePtr prevRasterizer = nullptr;
		mContext->RSGetState( &prevRasterizer );
		mContext->RSSetState( mDebugRasterizer );

		mTransformsBuffer.SetData( terrains.mTransforms );
		mTransformsBuffer.Bind( DX11CPUDynamicBuffer::Shaderslot::Vertex, SBUFFER_SLOT_EXTRA );

		uint32_t beginIndex = 0;
		for ( uint32_t terrainIndex = 0, numTerrains = terrains.GetNumTerrains(); terrainIndex < numTerrains; ++terrainIndex )
		{
			const RenderableTerrainData& terrainData = terrains.mTerrainData.at( terrainIndex );
			const DX11Texture& heightmap = mTextureMap.GetItem( terrainData.mHeightMap );

			heightmap.BindAsShaderResource( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA );

			mPerTerrainCBuffer.SetData( { beginIndex, terrainData.mHeightScale } );
			mPerTerrainCBuffer.Bind();

			uint32_t endIndex = terrainData.mEndIndex;
			assert( endIndex > beginIndex );

			uint32_t numTransforms = endIndex - beginIndex;
			mQuadMesh.DrawInstanced( numTransforms );

			beginIndex = endIndex;
		}

		mContext->RSSetState( prevRasterizer );

		UnbindRendering();
	}

	void DX11TerrainPass::BindForRendering()
	{
		mContext->VSSetShader(mVertexShader, nullptr, 0);
		mContext->DSSetShader(mDomainShader, nullptr, 0);
		mContext->PSSetShader(mPixelShader, nullptr, 0);
		mContext->DSSetShader(mDomainShader, nullptr, 0);
		mContext->HSSetShader(mHullShader, nullptr, 0);
		mContext->IASetInputLayout(mLayout);
		mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		mCBuffer.SetData({ mTessData.mMinDistance, mTessData.mMaxDistance, mTessData.mMinFactor, mTessData.mMaxFactor });
		mCBuffer.Bind();
	}

	void DX11TerrainPass::UnbindRendering()
	{
		mContext->DSSetShader(nullptr, nullptr, 0);
		mContext->HSSetShader(nullptr, nullptr, 0);
	}
}
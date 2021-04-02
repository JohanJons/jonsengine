#include "Renderer/DirectX11/DX11TerrainPass.h"

#include "Renderer/DirectX11/DX11Texture.h"
#include "Renderer/DirectX11/DX11VertexTransformPass.h"
#include "Renderer/RenderSettings.h"
#include "Compiled/TerrainVertex.h"
#include "Compiled/TerrainComputeNormal.h"
#include "Compiled/TerrainComputeAltitude.h"
#include "Compiled/TerrainComputeJFA.h"
#include "Compiled/TerrainComputeMoisture.h"
#include "Compiled/TerrainPixelDebug.h"
#include "Compiled/TerrainPixelTopographyDebug.h"
#include "Compiled/TerrainPixelMoistureDebug.h"
#include "Compiled/TerrainPixelNormalSimple.h"
#include "Compiled/TerrainPixelNormalSimpleDebug.h"
#include "Compiled/TerrainPixelNormalBetter.h"
#include "Compiled/TerrainPixelNormalBetterDebug.h"
#include "Core/Math/MathUtils.h"
#include "Core/Math/AABB.h"

namespace JonsEngine
{
	const std::vector<float> gQuadVertices{
		-0.5f, 0.0f, -0.5f,	// BL
		0.5f, 0.0f, -0.5f,	// BR
		0.5f, 0.0f, 0.5f,	// TR
		-0.5f, 0.0f, 0.5f,	// TL
	};

	const std::vector<uint16_t> gQuadIndices{ 0, 1, 2, 3 };

	enum VSInputLayout
	{
		POSITION = 0,
		NUM_INPUT_LAYOUTS
	};

	DX11TerrainPass::TerrainRenderData::TerrainRenderData( ID3D11DevicePtr device, ID3D11DeviceContextPtr context, uint32_t textureWidth, uint32_t textureHeight ) :
		mNormalMap( device, context, DXGI_FORMAT_R10G10B10A2_UNORM, textureWidth, textureHeight, true, true ),
		mTopographyMap( device, context, DXGI_FORMAT_R8G8B8A8_UINT, textureWidth, textureHeight, true, true ),
		mAverageAltitudeMap( device, context, DXGI_FORMAT_R32_UINT, 2, 2, true, true ),
		mJumpFloodRiverMap( device, context, DXGI_FORMAT_R16G16_UINT, textureWidth, textureHeight, true, true ),
		mMoistureMap( device, context, DXGI_FORMAT_R32_FLOAT, textureWidth, textureHeight, true, true )
	{
	}

	DX11TerrainPass::DX11TerrainPass( ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformer, const IDMap<DX11Texture>& textureMap,
		RenderSettings::TerrainMeshDimensions meshDimensions ):
		mContext( context ),
        mDevice( device ),
		mTextureMap( textureMap ),
		mCachedMeshDimensions( meshDimensions ),
		mPerTerrainCBuffer( device, context, mPerTerrainCBuffer.CONSTANT_BUFFER_SLOT_EXTRA ),
		mJFACBuffer( device, context, mJFACBuffer.CONSTANT_BUFFER_SLOT_EXTRA ),
		mMoistureCBuffer( device, context, mMoistureCBuffer.CONSTANT_BUFFER_SLOT_EXTRA ),
		mLODMorphConstantsBuffer( device, context ),
		mTransformBuffer( device, context ),
		mLODLevelBuffer( device, context )
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
		DXCALL( device->CreateInputLayout( inputDescription, VSInputLayout::NUM_INPUT_LAYOUTS, gTerrainVertex, sizeof( gTerrainVertex ), &mLayout ) );

		DXCALL( device->CreateVertexShader( gTerrainVertex, sizeof( gTerrainVertex ), nullptr, &mVertexShader ) );
		DXCALL( device->CreatePixelShader( gTerrainPixelNormalSimple, sizeof( gTerrainPixelNormalSimple ), nullptr, &mPixelNormalSimpleShader ) );
		DXCALL( device->CreatePixelShader( gTerrainPixelNormalSimpleDebug, sizeof( gTerrainPixelNormalSimpleDebug ), nullptr, &mPixelNormalSimpleShaderDebug ) );
		DXCALL( device->CreatePixelShader( gTerrainPixelNormalBetter, sizeof( gTerrainPixelNormalBetter ), nullptr, &mPixelNormalBetterShader ) );
		DXCALL( device->CreatePixelShader( gTerrainPixelNormalBetterDebug, sizeof( gTerrainPixelNormalBetterDebug ), nullptr, &mPixelNormalBetterShaderDebug ) );
		DXCALL( device->CreatePixelShader( gTerrainPixelDebug, sizeof( gTerrainPixelDebug ), nullptr, &mPixelCDLODDebugShader ) );
		DXCALL( device->CreatePixelShader( gTerrainPixelTopographyDebug, sizeof( gTerrainPixelTopographyDebug ), nullptr, &mPixelTopographyDebugShader ) );
		DXCALL( device->CreatePixelShader( gTerrainPixelMoistureDebug, sizeof( gTerrainPixelMoistureDebug ), nullptr, &mPixelMoistureDebugShader ) );
		DXCALL( device->CreateComputeShader( gTerrainComputeNormal, sizeof( gTerrainComputeNormal ), nullptr, &mNormalMapComputeShader ) );
		DXCALL( device->CreateComputeShader( gTerrainComputeAltitude, sizeof( gTerrainComputeAltitude ), nullptr, &mAltitudeComputeShader ) );
		DXCALL( device->CreateComputeShader( gTerrainComputeJFA, sizeof( gTerrainComputeJFA ), nullptr, &mJFAComputeShader ) );
		DXCALL( device->CreateComputeShader( gTerrainComputeMoisture, sizeof( gTerrainComputeMoisture ), nullptr, &mMoistureComputeShader ) );

		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory( &rasterizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );
		rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise = true;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.AntialiasedLineEnable = false;
		DXCALL( device->CreateRasterizerState( &rasterizerDesc, &mDebugRasterizer ) );

		CreateGridMesh( meshDimensions );
	}

	void DX11TerrainPass::Render( const RenderableTerrains& terrains, const RenderSettings& settings )
	{
		if ( !terrains.GetNumTerrains() )
			return;

		BindForRendering( settings.mTerrainNormals );
		RenderInternal( terrains, settings );
		UnbindRendering();
	}

	void DX11TerrainPass::RenderDebug( const RenderableTerrains& terrains, const RenderSettings& settings, DebugOptions::RenderingFlags debugFlags )
	{
		if ( !terrains.GetNumTerrains() )
			return;

		BindForRendering( settings.mTerrainNormals );

		bool drawNormals = debugFlags.test( DebugOptions::RenderingFlag::RENDER_FLAG_DRAW_TERRAIN_NORMAL );
		bool drawTopography = debugFlags.test( DebugOptions::RenderingFlag::RENDER_FLAG_DRAW_TERRAIN_TOPOGRAPHY );
		bool drawMoisture = debugFlags.test( DebugOptions::RenderingFlag::RENDER_FLAG_DRAW_TERRAIN_MOISTURE );

		if ( drawNormals )
		{
			switch ( settings.mTerrainNormals )
			{
				case RenderSettings::TerrainNormals::BETTER:
				{
					mContext->PSSetShader( mPixelNormalBetterShaderDebug, nullptr, 0 );
					break;
				}
				case RenderSettings::TerrainNormals::SIMPLE:
				default:
				{
					mContext->PSSetShader( mPixelNormalSimpleShaderDebug, nullptr, 0 );
					break;
				}
			}
		}
		else if ( drawTopography )
		{
			mContext->PSSetShader( mPixelTopographyDebugShader, nullptr, 0 );
		}
		else if ( drawMoisture )
		{
			mContext->PSSetShader( mPixelMoistureDebugShader, nullptr, 0 );
		}
		else
			mContext->PSSetShader( mPixelCDLODDebugShader, nullptr, 0 );

		ID3D11RasterizerStatePtr prevRasterizer = nullptr;
		bool drawWireframe = debugFlags.test( DebugOptions::RenderingFlag::RENDER_FLAG_DRAW_TERRAIN_WIREFRAME );
		if ( drawWireframe )
		{
			mContext->RSGetState( &prevRasterizer );
			mContext->RSSetState( mDebugRasterizer );
		}

		RenderInternal( terrains, settings );

		if ( drawWireframe )
			mContext->RSSetState( prevRasterizer );

		UnbindRendering();
	}

	void DX11TerrainPass::BindForRendering( RenderSettings::TerrainNormals normalSetting )
	{
		mContext->VSSetShader( mVertexShader, nullptr, 0 );

		switch ( normalSetting )
		{
			case RenderSettings::TerrainNormals::BETTER:
			{
				mContext->PSSetShader( mPixelNormalBetterShader, nullptr, 0 );
				break;
			}
			case RenderSettings::TerrainNormals::SIMPLE:
			default:
			{
				mContext->PSSetShader( mPixelNormalSimpleShader, nullptr, 0 );
				break;
			}
		}

		mContext->IASetInputLayout( mLayout );
		mContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	}

	void DX11TerrainPass::UnbindRendering()
	{
		mContext->VSSetShader( nullptr, nullptr, 0 );
		mContext->PSSetShader( nullptr, nullptr, 0 );
	}

	bool DX11TerrainPass::ShouldRecreateGridMesh( RenderSettings::TerrainMeshDimensions meshDimensions )
	{
		return mCachedMeshDimensions != meshDimensions;
	}

	void DX11TerrainPass::CreateGridMesh( RenderSettings::TerrainMeshDimensions meshDimensions )
	{
		if ( mCachedMeshDimensions != meshDimensions )
			mCachedMeshDimensions = meshDimensions;

		int32_t dimension = static_cast<int32_t>( RenderSettingsToVal( mCachedMeshDimensions ) );
		
		int32_t maxVertices = ( dimension + 1 ) * ( dimension + 1 ) * 3;
		int32_t maxIndices = dimension * dimension * 2 * 3;

		std::vector<float> vertices;
		std::vector<uint16_t> indices;
		vertices.reserve( maxVertices );
		indices.reserve( maxIndices );

		int32_t vertDim = dimension + 1;
		int32_t halfVertDim = vertDim / 2;

		for ( int32_t z = 0; z < vertDim; ++z )
		{
			for ( int32_t x = 0; x < vertDim; ++x )
			{
				float posX = x / static_cast<float>( dimension );
				float posZ = z / static_cast<float>( dimension );
				posX -= 0.5f;
				posZ -= 0.5f;

				vertices.insert( vertices.end(), { posX, 0.0f, posZ } );
			}
		}

		for ( int32_t z = 0; z < dimension; ++z )
		{
			for ( int32_t x = 0; x < dimension; ++x )
			{
				uint16_t v1 = x + vertDim * z;
				uint16_t v2 = ( x + 1 ) + vertDim * z;
				uint16_t v3 = x + vertDim * ( z + 1 );
				uint16_t v4 = ( x + 1 ) + vertDim * ( z + 1 );
				indices.insert( indices.end(), { v3, v2, v1, v3, v4, v2 } );
			}
		}

		Vec3 minBounds( -dimension / 2.0f, 0.0f, -dimension / 2.0f );
		Vec3 maxBounds( dimension / 2.0f, 0.0f, dimension / 2.0f );

		mGridMesh = DX11Mesh( mDevice, mContext, vertices, indices, minBounds, maxBounds );
	}

	void DX11TerrainPass::RenderInternal( const RenderableTerrains& terrains, const RenderSettings& settings )
	{
		if ( !terrains.GetNumTerrains() )
			return;

		if ( ShouldRecreateGridMesh( settings.mTerrainMeshDimensions ) )
			CreateGridMesh( settings.mTerrainMeshDimensions );

		mTransformBuffer.SetData( terrains.mTransforms );
		mTransformBuffer.Bind( DX11CPUDynamicBuffer::Shaderslot::Vertex, SBUFFER_SLOT_EXTRA_2 );

		mLODLevelBuffer.SetData( terrains.mLODs );
		mLODLevelBuffer.Bind( DX11CPUDynamicBuffer::Shaderslot::Vertex, SBUFFER_SLOT_EXTRA_3 );

		uint32_t beginIndex = 0;
		for ( const RenderableTerrainData& sceneTerrainData : terrains.mTerrainData )
		{
			TerrainRenderData& renderData = AccessOrCreateRenderData( sceneTerrainData );

			if ( ShouldUpdateBackendTerrainData( sceneTerrainData, renderData ) )
				UpdateRenderData( renderData, sceneTerrainData );

			const DX11DynamicTexture& normalMap = renderData.mNormalMap;
			const DX11DynamicTexture& topography = renderData.mTopographyMap;
			const DX11DynamicTexture& moisture = renderData.mMoistureMap;
			const DX11Texture& heightmap = mTextureMap.GetItem( sceneTerrainData.mHeightMap );
			heightmap.BindAsShaderResource( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA );
			normalMap.BindAsShaderResource( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_NORMAL );
			topography.BindAsShaderResource( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA_2 );
			moisture.BindAsShaderResource( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA_3 );

			mPerTerrainCBuffer.SetData( { sceneTerrainData.mWorldMin, sceneTerrainData.mWorldMax, sceneTerrainData.mHeightScale, sceneTerrainData.mVariationScale, beginIndex } );
			mPerTerrainCBuffer.Bind();

			mLODMorphConstantsBuffer.SetData( sceneTerrainData.mLODMorphConstants );
			mLODMorphConstantsBuffer.Bind( DX11CPUDynamicBuffer::Shaderslot::Vertex, SBUFFER_SLOT_EXTRA );

			uint32_t endIndex = sceneTerrainData.mEndIndex;
			assert( endIndex > beginIndex );

			uint32_t numTransforms = endIndex - beginIndex;

			mGridMesh.DrawInstanced( numTransforms );

			moisture.Unbind( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA_3 );
			topography.Unbind( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA_2 );
			normalMap.Unbind( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_NORMAL );
			heightmap.Unbind( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA );

			beginIndex = endIndex;
		}
	}

	DX11TerrainPass::TerrainRenderData& DX11TerrainPass::AccessOrCreateRenderData( const RenderableTerrainData& sceneTerrainData )
	{
		const DX11TextureID heightmapID = sceneTerrainData.mHeightMap;
		const DX11TextureID rivermapID = sceneTerrainData.mRiversMap;

		auto iterFind = mTerrainData.find( heightmapID );
		if ( iterFind != mTerrainData.cend() )
		{
			return iterFind->second;
		}

		uint32_t width, height;
		GetTextureDimensions( width, height, heightmapID );

		mTerrainData.erase( heightmapID );
		auto iterEmplace = mTerrainData.emplace( std::piecewise_construct, std::forward_as_tuple( heightmapID ), std::forward_as_tuple( mDevice, mContext, width, height ) );
		TerrainRenderData& renderData = iterEmplace.first->second;

		UpdateRenderData( renderData, sceneTerrainData );

		return renderData;
	}

	void DX11TerrainPass::GetTextureDimensions( uint32_t& width, uint32_t& height, DX11TextureID heightmapID )
	{
		const DX11Texture& heightTexture = mTextureMap.GetItem( heightmapID );

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof( D3D11_TEXTURE2D_DESC ) );
		heightTexture.GetDesc( desc );

		width = desc.Width;
		height = desc.Height;
	}

	void DX11TerrainPass::GetDispatchDimensions( uint32_t& x, uint32_t& y, DX11TextureID heightmapID )
	{
		uint32_t width, height;
		GetTextureDimensions( width, height, heightmapID );

		x = width / TERRAIN_NORMAL_THREADS_AXIS;
		y = height / TERRAIN_NORMAL_THREADS_AXIS;
	}

	bool DX11TerrainPass::ShouldUpdateBackendTerrainData(  const RenderableTerrainData& sceneTerrainData, const TerrainRenderData& renderTerrainData )
	{
		if ( renderTerrainData.mMoistureFalloffBegin != sceneTerrainData.mMoistureFalloffBegin || renderTerrainData.mMoistureFalloffDistance != sceneTerrainData.mMoistureFalloffDistance )
		{
			return true;
		}

		return false;
	}

	void DX11TerrainPass::UpdateRenderData( TerrainRenderData& renderData, const RenderableTerrainData& sceneTerrainData )
	{
		const DX11TextureID heightmapID = sceneTerrainData.mHeightMap;
		const DX11TextureID rivermapID = sceneTerrainData.mRiversMap;

		uint32_t width, height;
		GetTextureDimensions( width, height, heightmapID );

		uint32_t dispatchX, dispatchY;
		GetDispatchDimensions( dispatchX, dispatchY, heightmapID );

		UpdateNormalMapAndPrepJFA( renderData, heightmapID, rivermapID, dispatchX, dispatchY );
		UpdateAltitudeMaps(renderData, heightmapID, dispatchX, dispatchY );
		UpdateJFA( renderData, width, height, dispatchX, dispatchY );
		UpdateMoistureMap( renderData, heightmapID, sceneTerrainData.mMoistureFalloffBegin, sceneTerrainData.mMoistureFalloffDistance, sceneTerrainData.mHeightScale  );
	}

	void DX11TerrainPass::UpdateAltitudeMaps( TerrainRenderData& renderData, DX11TextureID heightmapID, uint32_t dispatchX, uint32_t dispatchY )
	{
		const DX11Texture& heightmapTexture = mTextureMap.GetItem( heightmapID );

		const DX11DynamicTexture& altitudeMap = renderData.mAverageAltitudeMap;

		mContext->CSSetShader( mAltitudeComputeShader, nullptr, 0 );

		heightmapTexture.BindAsShaderResource( SHADER_TEXTURE_SLOT_EXTRA );
		mContext->CSSetUnorderedAccessViews( UAV_SLOT_0, 1, &altitudeMap.mUAV.p, nullptr );

		mContext->Dispatch( dispatchX, dispatchY, 1 );

		mContext->CSSetUnorderedAccessViews( UAV_SLOT_0, 1, &gNullUAV.p, nullptr );
		heightmapTexture.Unbind( SHADER_TEXTURE_SLOT_EXTRA );
	}

	void DX11TerrainPass::UpdateNormalMapAndPrepJFA( TerrainRenderData& renderData, DX11TextureID heightmapID, DX11TextureID rivermapID, uint32_t dispatchX, uint32_t dispatchY )
	{
		const DX11Texture& heightmapTexture = mTextureMap.GetItem( heightmapID );

		const DX11DynamicTexture& normalMap = renderData.mNormalMap;
		const DX11DynamicTexture& JFARiverMap = renderData.mJumpFloodRiverMap;

		mContext->CSSetShader( mNormalMapComputeShader, nullptr, 0 );

		heightmapTexture.BindAsShaderResource( SHADER_TEXTURE_SLOT_EXTRA );

		if ( rivermapID != INVALID_DX11_TEXTURE_ID )
		{
			const DX11Texture& rivermapTexture = mTextureMap.GetItem( rivermapID );
			rivermapTexture.BindAsShaderResource( SHADER_TEXTURE_SLOT_EXTRA_2 );
		}

		mContext->CSSetUnorderedAccessViews( UAV_SLOT_0, 1, &normalMap.mUAV.p, nullptr );
		mContext->CSSetUnorderedAccessViews( UAV_SLOT_1, 1, &JFARiverMap.mUAV.p, nullptr );
		mContext->Dispatch( dispatchX, dispatchY, 1 );

		mContext->CSSetUnorderedAccessViews( UAV_SLOT_0, 1, &gNullUAV.p, nullptr );
		mContext->CSSetUnorderedAccessViews( UAV_SLOT_1, 1, &gNullUAV.p, nullptr );
		heightmapTexture.Unbind( SHADER_TEXTURE_SLOT_EXTRA );

		mContext->GenerateMips( normalMap.mSRV );
	}

	void DX11TerrainPass::UpdateJFA( TerrainRenderData& renderData, uint32_t width, uint32_t height, uint32_t dispatchX, uint32_t dispatchY )
	{
		const DX11DynamicTexture& JFARiverMap = renderData.mJumpFloodRiverMap;

		mContext->CSSetShader( mJFAComputeShader, nullptr, 0 );
		mJFACBuffer.Bind();
		mContext->CSSetUnorderedAccessViews( UAV_SLOT_0, 1, &JFARiverMap.mUAV.p, nullptr );

		int32_t logX = static_cast<int32_t>( std::log2( width ) );
		int32_t logY = static_cast<int32_t>( std::log2( height ) );

		int32_t numRuns = std::max( logX, logY );
		for ( int32_t runIndex = 0; runIndex < numRuns; ++runIndex )
		{
			int32_t offsetX = static_cast<int32_t>( std::pow( 2, logX - runIndex - 1 ) );
			int32_t offsetY = static_cast<int32_t>( std::pow( 2, logY - runIndex - 1 ) );

			mJFACBuffer.SetData( { offsetX, offsetY } );
			mContext->Dispatch( dispatchX, dispatchY, 1 );
		}

		mContext->CSSetUnorderedAccessViews( UAV_SLOT_0, 1, &gNullUAV.p, nullptr );
	}

	void DX11TerrainPass::UpdateMoistureMap( TerrainRenderData& renderData, DX11TextureID heightmapID, float moistureFalloffBegin, float moistureFalloffDistance, float heightScale )
	{
		const DX11Texture& heightmap = mTextureMap.GetItem( heightmapID );
		const DX11DynamicTexture& JFARiverMap = renderData.mJumpFloodRiverMap;
		const DX11DynamicTexture& moistureMap = renderData.mMoistureMap;

		mMoistureCBuffer.SetData( { moistureFalloffBegin, moistureFalloffDistance, heightScale } );
		mMoistureCBuffer.Bind();

		mContext->CSSetShader( mMoistureComputeShader, nullptr, 0 );
		heightmap.BindAsShaderResource( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA );
		JFARiverMap.BindAsShaderResource( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA_2 );
		mContext->CSSetUnorderedAccessViews( UAV_SLOT_0, 1, &moistureMap.mUAV.p, nullptr );

		uint32_t dispatchX, dispatchY;
		GetDispatchDimensions( dispatchX, dispatchY, heightmapID );
		mContext->Dispatch( dispatchX, dispatchY, 1 );

		mContext->CSSetUnorderedAccessViews( UAV_SLOT_0, 1, &gNullUAV.p, nullptr );
		heightmap.Unbind( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA );
		JFARiverMap.Unbind( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA_2 );

		renderData.mMoistureFalloffBegin = moistureFalloffBegin;
		renderData.mMoistureFalloffDistance = moistureFalloffDistance;
	}
}
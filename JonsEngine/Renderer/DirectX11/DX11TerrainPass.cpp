#include "Renderer/DirectX11/DX11TerrainPass.h"

#include "Renderer/DirectX11/DX11Texture.h"
#include "Renderer/DirectX11/DX11VertexTransformPass.h"
#include "Renderer/RenderSettings.h"
#include "Compiled/TerrainVertex.h"
#include "Compiled/TerrainComputeNormal.h"
#include "Compiled/TerrainPixelDebug.h"
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


	DX11TerrainPass::DX11TerrainPass( ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformer, const IDMap<DX11Texture>& textureMap,
		RenderSettings::TerrainMeshDimensions meshDimensions ):
		mContext( context ),
        mDevice( device ),
		mTextureMap( textureMap ),
		mCachedMeshDimensions( meshDimensions ),
		mPerTerrainCBuffer( device, context, mPerTerrainCBuffer.CONSTANT_BUFFER_SLOT_EXTRA ),
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
		DXCALL( device->CreateComputeShader( gTerrainComputeNormal, sizeof( gTerrainComputeNormal ), nullptr, &mNormalMapComputeShader ) );

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
		for ( const RenderableTerrainData& terrainData : terrains.mTerrainData )
		{
			const DX11Texture& heightmap = mTextureMap.GetItem( terrainData.mHeightMap );
			PerTerrainData& dx11TerrainData = GetOrCreateTerrainData( terrainData.mHeightMap );

			const DX11DynamicTexture& normalTexture = dx11TerrainData.mNormalMap;
			heightmap.BindAsShaderResource( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA );
			normalTexture.BindAsShaderResource( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_NORMAL );

			mPerTerrainCBuffer.SetData( { terrainData.mWorldMin, terrainData.mWorldMax, terrainData.mHeightScale, terrainData.mVariationScale, beginIndex } );
			mPerTerrainCBuffer.Bind();

			mLODMorphConstantsBuffer.SetData( terrainData.mLODMorphConstants );
			mLODMorphConstantsBuffer.Bind( DX11CPUDynamicBuffer::Shaderslot::Vertex, SBUFFER_SLOT_EXTRA );

			uint32_t endIndex = terrainData.mEndIndex;
			assert( endIndex > beginIndex );

			uint32_t numTransforms = endIndex - beginIndex;

			mGridMesh.DrawInstanced( numTransforms );

			normalTexture.Unbind( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_NORMAL );
			heightmap.Unbind( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA );

			beginIndex = endIndex;
		}
	}

	DX11TerrainPass::PerTerrainData& DX11TerrainPass::GetOrCreateTerrainData( DX11TextureID heightmapID )
	{
		if ( mTerrainData.find( heightmapID ) == mTerrainData.end() )
		{
			mTerrainData.emplace( heightmapID );
		}

		return mTerrainData.at( heightmapID );
	}

	bool DX11TerrainPass::HasCachedTextureMap( CachedTextureMap type, DX11TextureID heightmapID ) const
	{
		switch ( type )
		{
			case CachedTextureMap::NORMAL:  return mTerrainNormalMap.find( heightmapID ) != mTerrainNormalMap.cend();
			default:
			{
				assert( false && "Unhandled case" );
				return false;
			}
		}
	}

	void DX11TerrainPass::CreateTextureMap( CachedTextureMap type, DX11TextureID heightmapID )
	{
		uint32_t width, height;
		GetTextureMapDimensions( width, height, type, heightmapID );
		DXGI_FORMAT textureFormat = GetTextureMapFormat( type );

		auto& Map = GetTextureMap( type );
		Map.erase( heightmapID );
		Map.emplace( std::piecewise_construct, std::forward_as_tuple( heightmapID ), std::forward_as_tuple( mDevice, mContext, textureFormat, width, height, true, true ) );
		UpdateTextureMap( type, heightmapID );
	}

	void DX11TerrainPass::GetTextureMapDimensions( uint32_t& width, uint32_t& height, CachedTextureMap type, DX11TextureID heightmapID )
	{
		const DX11Texture& heightTexture = mTextureMap.GetItem( heightmapID );

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof( D3D11_TEXTURE2D_DESC ) );
		heightTexture.GetDesc( desc );

		switch ( type )
		{
			case CachedTextureMap::NORMAL:
			{
				width = desc.Width;
				height = desc.Height;
				break;
			}

			default:
				assert( false && "Unhandled case" );
		}
	}

	DXGI_FORMAT DX11TerrainPass::GetTextureMapFormat( CachedTextureMap type )
	{
		switch ( type )
		{
			case CachedTextureMap::NORMAL: return DXGI_FORMAT_R10G10B10A2_UNORM;
			default:
			{
				assert( false && "Unhandled case" );
				return DXGI_FORMAT();
			}
		}
	}

	std::map<DX11TextureID, DX11DynamicTexture>& DX11TerrainPass::GetTextureMap( CachedTextureMap type )
	{
		switch ( type )
		{
			case CachedTextureMap::NORMAL: return mTerrainNormalMap;
			default:
			{
				assert( false && "Unhandled case" );
				static std::map<DX11TextureID, DX11DynamicTexture> gEmptyMap;
				return gEmptyMap;
			}
		}
	}

	void DX11TerrainPass::UpdateTextureMap( CachedTextureMap type, DX11TextureID heightmapID )
	{
		const DX11Texture& heightmapTexture = mTextureMap.GetItem( heightmapID );
		const DX11DynamicTexture& textureMap = GetTextureMap( type ).at( heightmapID );

		BindComputeShader( type );

		uint32_t dispatchX, dispatchY;
		GetDispatchDimensions( dispatchX, dispatchY, type, heightmapID );

		heightmapTexture.BindAsShaderResource( SHADER_TEXTURE_SLOT_EXTRA );
		mContext->CSSetUnorderedAccessViews( UAV_SLOT, 1, &textureMap.mUAV.p, nullptr );
		mContext->Dispatch( dispatchX, dispatchY, 1 );
		mContext->CSSetUnorderedAccessViews( UAV_SLOT, 1, &gNullUAV.p, nullptr );
		heightmapTexture.Unbind( SHADER_TEXTURE_SLOT_EXTRA );

		mContext->GenerateMips( textureMap.mSRV );
	}

	void DX11TerrainPass::BindComputeShader( CachedTextureMap type )
	{
		switch ( type )
		{
			case CachedTextureMap::NORMAL:
			{
				mContext->CSSetShader( mNormalMapComputeShader, nullptr, 0 );
				break;
			}
			default:
				assert( false && "Unhandled case" );
		}
	}

	void DX11TerrainPass::GetDispatchDimensions( uint32_t& x, uint32_t& y, CachedTextureMap type, DX11TextureID heightmapID )
	{
		uint32_t width, height;
		GetTextureMapDimensions( width, height, type, heightmapID );

		switch ( type )
		{
			case CachedTextureMap::NORMAL:
			{
				x = width / TERRAIN_NORMAL_THREADS_AXIS;
				y = height / TERRAIN_NORMAL_THREADS_AXIS;
				break;
			}

			default:
				assert( false && "Unhandled case" );
		}
	}
}
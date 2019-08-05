#include "Renderer/DirectX11/DX11TerrainPass.h"

#include "Renderer/DirectX11/DX11Texture.h"
#include "Renderer/DirectX11/DX11VertexTransformPass.h"
#include "Renderer/RenderSettings.h"
#include "Compiled/TerrainVertex.h"
#include "Compiled/TerrainHull.h"
#include "Compiled/TerrainHullDebugCoplanarity.h"
#include "Compiled/TerrainComputeCoplanarity8.h"
#include "Compiled/TerrainComputeCoplanarity16.h"
#include "Compiled/TerrainComputeCoplanarity32.h"
#include "Compiled/TerrainComputeNormal.h"
#include "Compiled/TerrainDomain.h"
#include "Compiled/TerrainDomainDebugCoplanarity.h"
#include "Compiled/TerrainPixel.h"
#include "Compiled/TerrainPixelDebug.h"
#include "Compiled/TerrainPixelDebugCoplanarity.h"
#include "Compiled/TerrainPixelDebugNormal.h"
#include "Core/Math/MathUtils.h"
#include "Core/Math/AABB.h"

namespace JonsEngine
{
	/*const std::vector<float> gQuadVertices{
		-0.5f, 0.0f, -0.5f,
		0.5f, 0.0f, -0.5f,
		0.5f, 0.0f, 0.5f,
		-0.5f, 0.0f, 0.5f,
		-0.5f, 0.0f, -1.5f,
		0.5f, 0.0f, -1.5f,
		1.5f, 0.0f, -0.5f,
		1.5f, 0.0f, 0.5f,
		0.5f, 0.0f, 1.5f,
		-0.5f, 0.0f, 1.5f,
		-1.5f, 0.0f, 0.5f,
		-1.5f, 0.0f, -0.5f,
	};*/

	const std::vector<float> gQuadVertices{
		-0.5f, 0.0f, -0.5f,	// BL
		0.5f, 0.0f, -0.5f,	// BR
		0.5f, 0.0f, 0.5f,	// TR
		-0.5f, 0.0f, 0.5f,	// TL
		0.0f, 0.0f, -1.0f,	// bottom-center
		1.0f, 0.0f, 0.0f,	// Right-center
		0.0f, 0.0f, 1.0f,	// Top-center
		-1.0f, 0.0f, 0.0f	// Left-center
	}

	const std::vector<uint16_t> gQuadIndices{ 0, 1, 2, 3, 4, 5, 6, 7 };

	//const std::vector<uint16_t> gQuadIndices{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

	enum VSInputLayout
	{
		POSITION = 0,
		NUM_INPUT_LAYOUTS
	};


	DX11TerrainPass::DX11TerrainPass( ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformer, const IDMap<DX11Texture>& textureMap, RenderSettings::TerrainCoplanaritySize coplanaritySize ) :
		mContext(context),
        mDevice(device),
		mPerTerrainCBuffer(device, context, mPerTerrainCBuffer.CONSTANT_BUFFER_SLOT_DOMAIN),
		mTransformsBuffer( device, context ),
		mLODBuffer( device, context ),
		mQuadMesh( device, context, gQuadVertices, gQuadIndices, AABB::gUnitQuadAABB.Min(), AABB::gUnitQuadAABB.Max() ),
		mVertexTransformer( vertexTransformer ),
		mTextureMap( textureMap ),
		mCoplanaritySize( coplanaritySize )
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
		DXCALL(device->CreateInputLayout(inputDescription, VSInputLayout::NUM_INPUT_LAYOUTS, gTerrainVertex, sizeof(gTerrainVertex), &mLayout));

		// shaders
		DXCALL(device->CreateVertexShader(gTerrainVertex, sizeof(gTerrainVertex), nullptr, &mVertexShader));
		DXCALL(device->CreateHullShader(gTerrainHull, sizeof(gTerrainHull), nullptr, &mHullShader));
		DXCALL(device->CreateHullShader(gTerrainHullDebugCoplanarity, sizeof(gTerrainHullDebugCoplanarity), nullptr, &mHullShaderDebugCoplanarity));
        DXCALL(device->CreateComputeShader(gTerrainComputeCoplanarity16, sizeof(gTerrainComputeCoplanarity16), nullptr, &mCoplanarityComputeShader16));
        DXCALL(device->CreateComputeShader(gTerrainComputeCoplanarity32, sizeof(gTerrainComputeCoplanarity32), nullptr, &mCoplanarityComputeShader32));
		DXCALL(device->CreateComputeShader(gTerrainComputeNormal, sizeof(gTerrainComputeNormal), nullptr, &mNormalMapComputeShader));
		DXCALL(device->CreateDomainShader(gTerrainDomain, sizeof(gTerrainDomain), nullptr, &mDomainShader));
		DXCALL(device->CreateDomainShader(gTerrainDomainDebugCoplanarity, sizeof(gTerrainDomainDebugCoplanarity), nullptr, &mDomainDebugCoplanarityShader));
		DXCALL(device->CreatePixelShader(gTerrainPixel, sizeof(gTerrainPixel), nullptr, &mPixelShader));
		DXCALL( device->CreatePixelShader( gTerrainPixelDebug, sizeof( gTerrainPixelDebug ), nullptr, &mPixelDebugShader ) );
		DXCALL( device->CreatePixelShader( gTerrainPixelDebugCoplanarity, sizeof( gTerrainPixelDebugCoplanarity ), nullptr, &mPixelDebugCoplanarityShader ) );
		DXCALL( device->CreatePixelShader( gTerrainPixelDebugNormal, sizeof( gTerrainPixelDebugNormal ), nullptr, &mPixelDebugNormalShader ) );

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

	void DX11TerrainPass::Render( const RenderableTerrains& terrains, RenderSettings::TerrainCoplanaritySize coplanaritySize )
	{
		if ( !terrains.GetNumTerrains() )
			return;

		BindForRendering();
		RenderInternal( terrains, coplanaritySize );
		UnbindRendering();
	}

	void DX11TerrainPass::RenderDebug( const RenderableTerrains& terrains, RenderSettings::TerrainCoplanaritySize coplanaritySize, DebugOptions::RenderingFlags debugFlags )
	{
		if ( !terrains.GetNumTerrains() )
			return;

		BindForRendering();

		bool drawCoplanarity = debugFlags.test( DebugOptions::RenderingFlag::RENDER_FLAG_DRAW_TERRAIN_COPLANARITY );
		bool drawNormals = debugFlags.test( DebugOptions::RenderingFlag::RENDER_FLAG_DRAW_TERRAIN_NORMAL );
		if ( drawCoplanarity )
		{
			mContext->HSSetShader( mHullShaderDebugCoplanarity, nullptr, 0 );
			mContext->DSSetShader( mDomainDebugCoplanarityShader, nullptr, 0 );
			mContext->PSSetShader( mPixelDebugCoplanarityShader, nullptr, 0 );
		}
		else if ( drawNormals )
		{
			mContext->PSSetShader( mPixelDebugNormalShader, nullptr, 0 );
		}
		else
			mContext->PSSetShader( mPixelDebugShader, nullptr, 0 );

		ID3D11RasterizerStatePtr prevRasterizer = nullptr;
		mContext->RSGetState( &prevRasterizer );
		mContext->RSSetState( mDebugRasterizer );

		RenderInternal( terrains, coplanaritySize );

		mContext->RSSetState( prevRasterizer );

		UnbindRendering();
	}

    void DX11TerrainPass::UpdatePatchSize( RenderSettings::TerrainCoplanaritySize coplanaritySize )
    {
        mCoplanaritySize = coplanaritySize;

        std::vector<DX11TextureID> heightTextures;
        for ( const auto& pair : mTerrainCoplanarityMap )
            heightTextures.emplace_back( pair.first );

        mTerrainCoplanarityMap.clear();
		for ( DX11TextureID ID : heightTextures )
		{
			CreateTextureMap( CachedTextureMap::COPLANARITY, ID );
			CreateTextureMap( CachedTextureMap::NORMAL, ID );
		}
    }

	void DX11TerrainPass::RenderInternal( const RenderableTerrains& terrains, RenderSettings::TerrainCoplanaritySize coplanaritySize )
	{
        if ( coplanaritySize != mCoplanaritySize )
            UpdatePatchSize( coplanaritySize );

		mTransformsBuffer.SetData( terrains.mTransforms );
		mTransformsBuffer.Bind( DX11CPUDynamicBuffer::Shaderslot::Vertex, SBUFFER_SLOT_EXTRA );

		uint32_t beginIndex = 0;
		for ( const RenderableTerrainData& terrainData : terrains.mTerrainData )
		{
            if ( !HasCachedTextureMap( CachedTextureMap::COPLANARITY, terrainData.mHeightMap ) )
				CreateTextureMap( CachedTextureMap::COPLANARITY, terrainData.mHeightMap );

			if ( !HasCachedTextureMap( CachedTextureMap::NORMAL, terrainData.mHeightMap ) )
				CreateTextureMap( CachedTextureMap::NORMAL, terrainData.mHeightMap );

			const DX11Texture& heightmap = mTextureMap.GetItem( terrainData.mHeightMap );
            const DX11DynamicTexture& coplanarityTexture = mTerrainCoplanarityMap.at( terrainData.mHeightMap );
			const DX11DynamicTexture& normalTexture = mTerrainNormalMap.at( terrainData.mHeightMap );

			heightmap.BindAsShaderResource( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA );
            coplanarityTexture.BindAsShaderResource( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA_2 );
			normalTexture.BindAsShaderResource( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_NORMAL );

			mPerTerrainCBuffer.SetData( { terrainData.mWorldMin, terrainData.mWorldMax, terrainData.mHeightScale, terrainData.mVariationScale, beginIndex, static_cast<uint32_t>( terrainData.mLODRanges.size() ) } );
			mPerTerrainCBuffer.Bind();

			mLODBuffer.SetData( terrainData.mLODRanges );
			mLODBuffer.Bind( DX11CPUDynamicBuffer::Shaderslot::Hull, SBUFFER_SLOT_EXTRA_2 );

			uint32_t endIndex = terrainData.mEndIndex;
			assert( endIndex > beginIndex );

			uint32_t numTransforms = endIndex - beginIndex;
			mQuadMesh.DrawInstanced( numTransforms );

			beginIndex = endIndex;

            heightmap.Unbind( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA );
            coplanarityTexture.Unbind( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_EXTRA_2 );
			coplanarityTexture.Unbind( SHADER_TEXTURE_SLOT::SHADER_TEXTURE_SLOT_NORMAL );
		}
	}

	void DX11TerrainPass::CreateTextureMap( CachedTextureMap type, DX11TextureID heightmapID )
	{
		uint32_t width, height;
		GetTextureMapDimensions( width, height, type, heightmapID );
		DXGI_FORMAT textureFormat = GetTextureMapFormat( type );

		auto& Map = GetTextureMap( type );
		Map.erase( heightmapID );
		Map.emplace( std::piecewise_construct, std::forward_as_tuple( heightmapID ), std::forward_as_tuple( mDevice, mContext, textureFormat, width, height, true ) );
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
			case CachedTextureMap::COPLANARITY:
			{
				uint32_t patchSizeVal = RenderSettingsToVal( mCoplanaritySize );
				assert( patchSizeVal > 0 );
				assert( desc.Width % patchSizeVal == 0 );
				assert( desc.Height % patchSizeVal == 0 );

				width = desc.Width / patchSizeVal;
				height = desc.Height / patchSizeVal;
				break;
			}

			case CachedTextureMap::NORMAL:
			{
				width = desc.Width;
				height = desc.Height;
				break;
			}
		}
	}

	DXGI_FORMAT DX11TerrainPass::GetTextureMapFormat( CachedTextureMap type )
	{
		switch ( type )
		{
			case CachedTextureMap::COPLANARITY: return DXGI_FORMAT_R16_UNORM;
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
			case CachedTextureMap::COPLANARITY: return mTerrainCoplanarityMap;
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
	}

	void DX11TerrainPass::BindComputeShader( CachedTextureMap type )
	{
		switch ( type )
		{
			case CachedTextureMap::COPLANARITY:
			{
				switch ( mCoplanaritySize )
				{
					case RenderSettings::TerrainCoplanaritySize::X8:
					{
						mContext->CSSetShader( mCoplanarityComputeShader8, nullptr, 0 );
						break;
					}

					case RenderSettings::TerrainCoplanaritySize::X16:
					{
						mContext->CSSetShader( mCoplanarityComputeShader16, nullptr, 0 );
						break;
					}

					case RenderSettings::TerrainCoplanaritySize::X32:
					{
						mContext->CSSetShader( mCoplanarityComputeShader32, nullptr, 0 );
						break;
					}
				}
				break;
			}

			case CachedTextureMap::NORMAL:
			{
				mContext->CSSetShader( mNormalMapComputeShader, nullptr, 0 );
				break;
			}
		}
	}

	void DX11TerrainPass::GetDispatchDimensions( uint32_t& x, uint32_t& y, CachedTextureMap type, DX11TextureID heightmapID )
	{
		uint32_t width, height;
		GetTextureMapDimensions( width, height, type, heightmapID );

		switch ( type )
		{
			case CachedTextureMap::COPLANARITY:
			{
				x = width;
				y = height;
				break;
			}

			case CachedTextureMap::NORMAL:
			{
				x = width / TERRAIN_NORMAL_THREADS_AXIS;
				y = height / TERRAIN_NORMAL_THREADS_AXIS;
				break;
			}
		}
	}

	void DX11TerrainPass::BindForRendering()
	{
		mContext->VSSetShader(mVertexShader, nullptr, 0);
		mContext->DSSetShader(mDomainShader, nullptr, 0);
		mContext->PSSetShader(mPixelShader, nullptr, 0);
		mContext->HSSetShader(mHullShader, nullptr, 0);

		mContext->IASetInputLayout(mLayout);
		mContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_12_CONTROL_POINT_PATCHLIST);
	}

	void DX11TerrainPass::UnbindRendering()
	{
		mContext->VSSetShader( nullptr, nullptr, 0 );
		mContext->DSSetShader( nullptr, nullptr, 0 );
		mContext->HSSetShader( nullptr, nullptr, 0 );
		mContext->PSSetShader( nullptr, nullptr, 0 );
	}

    bool DX11TerrainPass::HasCachedTextureMap( CachedTextureMap type, DX11TextureID heightmapID ) const
    {
		switch ( type )
		{
			case CachedTextureMap::COPLANARITY:  return mTerrainCoplanarityMap.find( heightmapID ) != mTerrainCoplanarityMap.cend();
			case CachedTextureMap::NORMAL:  return mTerrainNormalMap.find( heightmapID ) != mTerrainNormalMap.cend();
			default:
			{
				assert( false && "Unhandled case" );
				return false;
			}
		}
    }
}
#pragma once

#include "Renderer/DirectX11/DX11Utils.h"
#include "Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "Renderer/DirectX11/DX11CPUDynamicBuffer.h"
#include "Renderer/DirectX11/DX11DynamicTexture.h"
#include "Renderer/DirectX11/DX11Mesh.h"
#include "Renderer/RenderDefs.h"
#include "Renderer/RenderSettings.h"
#include "Core/Containers/IDMap.hpp"
#include "Core/DebugOptions.h"
#include "Core/Types.h"

#include <array>
#include <map>

namespace JonsEngine
{
	class DX11Texture;
	class DX11VertexTransformPass;
	struct RenderableTerrains;
    struct RenderSettings;

	class DX11TerrainPass
	{
	public:
		DX11TerrainPass( ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformer, const IDMap<DX11Texture>& textureMap,
			RenderSettings::TerrainPatchSize patchSize, RenderSettings::TerrainPatchVerticeRatio vertexRatio );

		void Render( const RenderableTerrains& terrains, RenderSettings::TerrainPatchSize patchSize, RenderSettings::TerrainPatchVerticeRatio vertexRatio );
		void RenderDebug( const RenderableTerrains& terrains, RenderSettings::TerrainPatchSize patchSize, RenderSettings::TerrainPatchVerticeRatio vertexRatio, DebugOptions::RenderingFlags debugFlags );

		//void Render( const RenderableTerrains& terrains, RenderSettings::TerrainCoplanaritySize coplanaritySize );
		//void RenderDebug( const RenderableTerrains& terrains, RenderSettings::TerrainCoplanaritySize coplanaritySize, DebugOptions::RenderingFlags debugFlags );

	private:
		struct PerTerrainCBuffer
		{
			Vec2 mWorldMin;
			Vec2 mWorldMax;
			float mHeightScale;
			float mVariationScale;
			uint32_t mTransformOffset;
			uint32_t __padding;
		};

	private:
		enum class CachedTextureMap
		{
			COPLANARITY,
			NORMAL
		};

		void BindForRendering();
		void UnbindRendering();

		bool ShouldRecreateGridMesh( RenderSettings::TerrainPatchSize newPatchSize, RenderSettings::TerrainPatchVerticeRatio newVertexRatio );
		void CreateGridMesh( RenderSettings::TerrainPatchSize newPatchSize, RenderSettings::TerrainPatchVerticeRatio newVertexRatio );
		void RenderInternal( const RenderableTerrains& terrains, RenderSettings::TerrainPatchSize patchSize, RenderSettings::TerrainPatchVerticeRatio vertexRatio );

		bool HasCachedTextureMap( CachedTextureMap type, DX11TextureID heightmapID ) const;
		void CreateTextureMap( CachedTextureMap type, DX11TextureID heightmapID );
		void GetTextureMapDimensions( uint32_t& width, uint32_t& height, CachedTextureMap type, DX11TextureID heightmapID );
		DXGI_FORMAT GetTextureMapFormat( CachedTextureMap type );
		std::map<DX11TextureID, DX11DynamicTexture>& GetTextureMap( CachedTextureMap type );
		void UpdateTextureMap( CachedTextureMap type, DX11TextureID heightmapID );
		void BindComputeShader( CachedTextureMap type );
		void GetDispatchDimensions( uint32_t& x, uint32_t& y, CachedTextureMap type, DX11TextureID heightmapID );

        //void UpdatePatchSize( RenderSettings::TerrainCoplanaritySize coplanaritySize );
		//void BindForRendering();
		//void UnbindRendering();

		ID3D11DeviceContextPtr mContext = nullptr;
        ID3D11DevicePtr mDevice = nullptr;
		const IDMap<DX11Texture>& mTextureMap;
		ID3D11InputLayoutPtr mLayout = nullptr;
		ID3D11VertexShaderPtr mVertexShader = nullptr;
		ID3D11PixelShaderPtr mPixelShader = nullptr;
		ID3D11PixelShaderPtr mPixelDebugShader = nullptr;
		ID3D11RasterizerStatePtr mDebugRasterizer = nullptr;
		ID3D11ComputeShaderPtr mNormalMapComputeShader = nullptr;

		RenderSettings::TerrainPatchSize mCachedPatchSize;
		RenderSettings::TerrainPatchVerticeRatio mCachedPatchVertexRatio;
		std::map<DX11TextureID, DX11DynamicTexture> mTerrainNormalMap;
		DX11Mesh mGridMesh;

		DX11ConstantBuffer<PerTerrainCBuffer> mPerTerrainCBuffer;
		DX11CPUDynamicBuffer mLODMorphConstantsBuffer;
		DX11CPUDynamicBuffer mTransformBuffer;

		/*ID3D11InputLayoutPtr mLayout = nullptr;
		ID3D11VertexShaderPtr mVertexShader = nullptr;
		ID3D11HullShaderPtr mHullShader = nullptr;
		ID3D11HullShaderPtr mHullShaderDebugCoplanarity = nullptr;
		ID3D11ComputeShaderPtr mCoplanarityComputeShader8 = nullptr;
        ID3D11ComputeShaderPtr mCoplanarityComputeShader16 = nullptr;
        ID3D11ComputeShaderPtr mCoplanarityComputeShader32 = nullptr;
		ID3D11ComputeShaderPtr mNormalMapComputeShader = nullptr;
		ID3D11DomainShaderPtr mDomainShader = nullptr;
		ID3D11DomainShaderPtr mDomainDebugCoplanarityShader = nullptr;
		ID3D11PixelShaderPtr mPixelShader = nullptr;
		ID3D11PixelShaderPtr mPixelDebugShader = nullptr;
		ID3D11PixelShaderPtr mPixelDebugCoplanarityShader = nullptr;
		ID3D11PixelShaderPtr mPixelDebugNormalShader = nullptr;

		ID3D11RasterizerStatePtr mDebugRasterizer = nullptr;
		*/

       /* std::map<DX11TextureID, DX11DynamicTexture> mTerrainCoplanarityMap;
		std::map<DX11TextureID, DX11DynamicTexture> mTerrainNormalMap;

		DX11ConstantBuffer<PerTerrainCBuffer> mPerTerrainCBuffer;
		DX11CPUDynamicBuffer mTransformsBuffer;
		DX11CPUDynamicBuffer mTessEdgeMultBuffer;
		DX11Mesh mQuadMesh;
		DX11VertexTransformPass& mVertexTransformer;
		const IDMap<DX11Texture>& mTextureMap;

        RenderSettings::TerrainCoplanaritySize mCoplanaritySize;*/
	};
}
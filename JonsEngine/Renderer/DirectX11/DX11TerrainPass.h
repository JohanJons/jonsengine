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
			RenderSettings::TerrainMeshDimensions meshDimensions );

		void Render( const RenderableTerrains& terrains, const RenderSettings& settings );
		void RenderDebug( const RenderableTerrains& terrains, const RenderSettings& settings, DebugOptions::RenderingFlags debugFlags );

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
			NORMAL
		};

		void BindForRendering( RenderSettings::TerrainNormals normalSetting );
		void UnbindRendering();

		bool ShouldRecreateGridMesh( RenderSettings::TerrainMeshDimensions meshDimensions );
		void CreateGridMesh( RenderSettings::TerrainMeshDimensions meshDimensions );
		void RenderInternal( const RenderableTerrains& terrains, const RenderSettings& settings );

		bool HasCachedTextureMap( CachedTextureMap type, DX11TextureID heightmapID ) const;
		void CreateTextureMap( CachedTextureMap type, DX11TextureID heightmapID );
		void GetTextureMapDimensions( uint32_t& width, uint32_t& height, CachedTextureMap type, DX11TextureID heightmapID );
		DXGI_FORMAT GetTextureMapFormat( CachedTextureMap type );
		std::map<DX11TextureID, DX11DynamicTexture>& GetTextureMap( CachedTextureMap type );
		void UpdateTextureMap( CachedTextureMap type, DX11TextureID heightmapID );
		void BindComputeShader( CachedTextureMap type );
		void GetDispatchDimensions( uint32_t& x, uint32_t& y, CachedTextureMap type, DX11TextureID heightmapID );

		ID3D11DeviceContextPtr mContext = nullptr;
        ID3D11DevicePtr mDevice = nullptr;
		const IDMap<DX11Texture>& mTextureMap;
		ID3D11InputLayoutPtr mLayout = nullptr;
		ID3D11VertexShaderPtr mVertexShader = nullptr;
		ID3D11PixelShaderPtr mPixelNormalSimpleShader = nullptr;
		ID3D11PixelShaderPtr mPixelNormalSimpleShaderDebug = nullptr;
		ID3D11PixelShaderPtr mPixelNormalBetterShader = nullptr;
		ID3D11PixelShaderPtr mPixelNormalBetterShaderDebug = nullptr;
		ID3D11PixelShaderPtr mPixelCDLODDebugShader = nullptr;
		ID3D11RasterizerStatePtr mDebugRasterizer = nullptr;
		ID3D11ComputeShaderPtr mNormalMapComputeShader = nullptr;

		RenderSettings::TerrainMeshDimensions mCachedMeshDimensions;
		std::map<DX11TextureID, DX11DynamicTexture> mTerrainNormalMap;
		DX11Mesh mGridMesh;

		DX11ConstantBuffer<PerTerrainCBuffer> mPerTerrainCBuffer;
		DX11CPUDynamicBuffer mLODMorphConstantsBuffer;
		DX11CPUDynamicBuffer mTransformBuffer;
		DX11CPUDynamicBuffer mLODLevelBuffer;
	};
}
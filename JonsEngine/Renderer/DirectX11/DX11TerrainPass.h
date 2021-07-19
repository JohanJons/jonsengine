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
	struct RenderableTerrainData;
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

		struct JFACBuffer
		{
			int32_t mOffsetX;
			int32_t mOffsetY;
			uint32_t __padding[ 2 ];
		};

		struct AverageAltitudeCBuffer
		{
			uint32_t mDispatchX;
			uint32_t mDispatchY;
			uint32_t __padding[ 2 ];
		};

		struct AltitudeKernelCBuffer
		{
			int32_t mKernelSize;
			uint32_t __padding[ 3 ];
		};

		struct MoistureCBuffer
		{
			float mDistanceFalloffBegin;
			float mDistanceFalloffDistance;
			float mHeightScale;
			uint32_t __padding;
		};

		struct TerrainRenderData
		{
			TerrainRenderData( ID3D11DevicePtr device, ID3D11DeviceContextPtr context, uint32_t textureWidth, uint32_t textureHeight );

			float mMoistureFalloffBegin = 0.0f;
			float mMoistureFalloffDistance = 0.0f;

			DX11DynamicTexture mNormalMap;
			DX11DynamicTexture mTopographyMap;
			DX11DynamicTexture mAltitudeMap;
			DX11DynamicTexture mAverageAltitude;
			DX11DynamicTexture mJumpFloodRiverMap;
			DX11DynamicTexture mMoistureMap;
		};

	private:
		void BindForRendering( RenderSettings::TerrainNormals normalSetting );
		void UnbindRendering();

		bool ShouldRecreateGridMesh( RenderSettings::TerrainMeshDimensions meshDimensions );
		void CreateGridMesh( RenderSettings::TerrainMeshDimensions meshDimensions );
		void RenderInternal( const RenderableTerrains& terrains, const RenderSettings& settings );

		TerrainRenderData& AccessOrCreateRenderData( const RenderableTerrainData& terrainData );

		void GetTextureDimensions( uint32_t& width, uint32_t& height, DX11TextureID heightmapID );
		void GetDispatchDimensions( uint32_t& x, uint32_t& y, DX11TextureID heightmapID );

		bool ShouldUpdateBackendTerrainData( const RenderableTerrainData& sceneTerrainData, const TerrainRenderData& renderTerrainData );
		void UpdateRenderData( TerrainRenderData& renderData, const RenderableTerrainData& sceneTerrainData );
		void UpdateAltitudeMaps( TerrainRenderData& renderData, DX11TextureID heightmapID, uint32_t dispatchX, uint32_t dispatchY );
		void UpdateNormalMapAndPrepJFA( TerrainRenderData& renderData, DX11TextureID heightmapID, DX11TextureID rivermapID, uint32_t dispatchX, uint32_t dispatchY );
		void UpdateJFA( TerrainRenderData& renderData, uint32_t width, uint32_t height, uint32_t dispatchX, uint32_t dispatchY );
		void UpdateMoistureMap( TerrainRenderData& renderData, DX11TextureID heightmapID, float moistureFalloffBegin, float moistureFalloffDistance, float heightScale );


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
		ID3D11PixelShaderPtr mPixelTopographyDebugShader = nullptr;
		ID3D11PixelShaderPtr mPixelMoistureDebugShader = nullptr;
		ID3D11RasterizerStatePtr mDebugRasterizer = nullptr;
		ID3D11ComputeShaderPtr mNormalMapComputeShader = nullptr;
		ID3D11ComputeShaderPtr mAltitudeComputeShader = nullptr;
		ID3D11ComputeShaderPtr mAverageAltitudeComputeShader = nullptr;
		ID3D11ComputeShaderPtr mAverageAltitudeFinalComputeShader = nullptr;
		ID3D11ComputeShaderPtr mJFAComputeShader = nullptr;
		ID3D11ComputeShaderPtr mMoistureComputeShader = nullptr;

		RenderSettings::TerrainMeshDimensions mCachedMeshDimensions;
		std::map<DX11TextureID, TerrainRenderData> mTerrainData;
		DX11Mesh mGridMesh;

		DX11ConstantBuffer<PerTerrainCBuffer> mPerTerrainCBuffer;
		DX11ConstantBuffer<JFACBuffer> mJFACBuffer;
		DX11ConstantBuffer<MoistureCBuffer> mMoistureCBuffer;
		DX11ConstantBuffer<AverageAltitudeCBuffer> mAvgAltitudeCBuffer;
		DX11ConstantBuffer<AltitudeKernelCBuffer> mAltitudeKernelCBuffer;
		DX11CPUDynamicBuffer mLODMorphConstantsBuffer;
		DX11CPUDynamicBuffer mTransformBuffer;
		DX11CPUDynamicBuffer mLODLevelBuffer;
	};
}
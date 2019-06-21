#pragma once

#include "Renderer/DirectX11/DX11Utils.h"
#include "Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "Renderer/DirectX11/DX11CPUDynamicBuffer.h"
#include "Renderer/DirectX11/DX11DynamicTexture.h"
#include "Renderer/DirectX11/DX11Mesh.h"
#include "Renderer/RenderSettings.h"
#include "Core/Containers/IDMap.hpp"

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
		DX11TerrainPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformer, const IDMap<DX11Texture>& textureMap, RenderSettings::TerrainPatchSize patchSize);

		void Render( const RenderableTerrains& terrains, RenderSettings::TerrainPatchSize patchSize );
		void RenderDebug( const RenderableTerrains& terrains, RenderSettings::TerrainPatchSize patchSize, bool drawCoplanarity );

	private:
		struct TerrainCBuffer
		{
			float mMinDistance;
			float mMaxDistance;
			float mMinFactor;
			float mMaxFactor;
		};

		struct PerTerrainCBuffer
		{
			Vec2 mWorldMin;
			Vec2 mWorldMax;
			float mHeightScale;
			float mVariationScale;
			uint32_t mTransformOffset;
			float __padding[1];
		};

	private:
		void RenderInternal( const RenderableTerrains& terrains, RenderSettings::TerrainPatchSize patchSize );
        void UpdateCoplanarityTexture( DX11TextureID heightmapID );
        void CreateCoplanarityMap( DX11TextureID heightmapID );
        void UpdatePatchSize( RenderSettings::TerrainPatchSize patchSize );
        void BindComputeShader();
		void BindForRendering( bool drawCoplanarity = false );
		void UnbindRendering();
        bool HasCoplanarityTexture( DX11TextureID heightmapID ) const;

		ID3D11DeviceContextPtr mContext = nullptr;
        ID3D11DevicePtr mDevice = nullptr;
		ID3D11InputLayoutPtr mLayout = nullptr;
		ID3D11VertexShaderPtr mVertexShader = nullptr;
		ID3D11HullShaderPtr mHullShader = nullptr;
		ID3D11HullShaderPtr mHullShaderDebugCoplanarity = nullptr;
        ID3D11ComputeShaderPtr mCoplanarityComputeShader16 = nullptr;
        ID3D11ComputeShaderPtr mCoplanarityComputeShader32 = nullptr;
		ID3D11DomainShaderPtr mDomainShader = nullptr;
		ID3D11PixelShaderPtr mPixelShader = nullptr;
		ID3D11PixelShaderPtr mPixelDebugShader = nullptr;
		ID3D11PixelShaderPtr mPixelDebugCoplanarityShader = nullptr;

		ID3D11RasterizerStatePtr mDebugRasterizer = nullptr;

        std::map<DX11TextureID, DX11DynamicTexture> mTerrainCoplanarityMap;

		DX11ConstantBuffer<TerrainCBuffer> mCBuffer;
		DX11ConstantBuffer<PerTerrainCBuffer> mPerTerrainCBuffer;
		DX11CPUDynamicBuffer mTransformsBuffer;
		DX11Mesh mQuadMesh;
		DX11VertexTransformPass& mVertexTransformer;
		const IDMap<DX11Texture>& mTextureMap;

        RenderSettings::TerrainPatchSize mPatchSize;
		RenderSettings::Tesselation mTessData; // TODO: remove
	};
}
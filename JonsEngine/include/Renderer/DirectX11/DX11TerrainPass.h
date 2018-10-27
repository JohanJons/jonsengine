#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11CPUDynamicBuffer.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/RenderSettings.h"
#include "include/Core/Containers/IDMap.hpp"

#include <array>

namespace JonsEngine
{
	class DX11Texture;
	class DX11VertexTransformPass;
	struct RenderableTerrains;

	class DX11TerrainPass
	{
	public:
		static constexpr int32_t GRID_WIDTH_IN_PATCHES = 8;
		static constexpr int32_t GRID_HEIGHT_IN_PATCHES = 8;
		static constexpr int32_t GRID_SIZE = GRID_WIDTH_IN_PATCHES * GRID_HEIGHT_IN_PATCHES;

		DX11TerrainPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformer, const IDMap<DX11Texture>& textureMap, const RenderSettings::Tesselation& tessData);

		void Render( const RenderableTerrains& terrains );
		void RenderDebug( const RenderableTerrains& terrains );

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
			float mHeightScale;
			Vec2 mWorldMin;
			Vec2 mWorldMax;
			float __padding[3];
		};

	private:
		void RenderInternal( const RenderableTerrains& terrains );
		void BindForRendering();
		void UnbindRendering();

		ID3D11DeviceContextPtr mContext = nullptr;
		ID3D11InputLayoutPtr mLayout = nullptr;
		ID3D11VertexShaderPtr mVertexShader = nullptr;
		ID3D11HullShaderPtr mHullShader = nullptr;
		ID3D11DomainShaderPtr mDomainShader = nullptr;
		ID3D11PixelShaderPtr mPixelShader = nullptr;
		ID3D11PixelShaderPtr mPixelDebugShader = nullptr;

		ID3D11RasterizerStatePtr mDebugRasterizer = nullptr;

		DX11ConstantBuffer<TerrainCBuffer> mCBuffer;
		DX11ConstantBuffer<PerTerrainCBuffer> mPerTerrainCBuffer;
		DX11CPUDynamicBuffer mTransformsBuffer;
		DX11Mesh mQuadMesh;
		DX11VertexTransformPass& mVertexTransformer;
		const IDMap<DX11Texture>& mTextureMap;

		RenderSettings::Tesselation mTessData;
	};
}
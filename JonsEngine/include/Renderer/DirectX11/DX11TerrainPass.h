#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11ConstantBuffer.hpp"
#include "include/Renderer/DirectX11/DX11CPUDynamicBuffer.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/RenderSettings.h"

#include <array>

namespace JonsEngine
{
	class DX11Texture;
	class DX11VertexTransformPass;

	class DX11TerrainPass
	{
	public:
		static constexpr uint32_t GRID_WIDTH_IN_PATCHES = 8;
		static constexpr uint32_t GRID_HEIGHT_IN_PATCHES = 8;
		static constexpr uint32_t GRID_SIZE = GRID_WIDTH_IN_PATCHES * GRID_HEIGHT_IN_PATCHES;

		DX11TerrainPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformer, const RenderSettings::Tesselation& tessData);
		~DX11TerrainPass();

		void Render(DX11Texture& heightmap, const Mat4& worldTransform, float heightScale, float patchSize, const Mat4& view, const Mat4& viewProjection);
		void BindForRendering();


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
			Mat4 viewProjection;
			Mat4 view;
			float mHeightScale;
			float mWorldPosMinX;
			float mWorldPosMinZ;
			float mWorldPosMaxX;
			float mWorldPosMaxZ;
		};



		ID3D11DeviceContextPtr mContext;
		ID3D11InputLayoutPtr mLayout;
		ID3D11VertexShaderPtr mVertexShader;
		ID3D11HullShaderPtr mHullShader;
		ID3D11DomainShaderPtr mDomainShader;
		ID3D11PixelShaderPtr mPixelShader;

		DX11ConstantBuffer<TerrainCBuffer> mCBuffer;
		DX11ConstantBuffer<PerTerrainCBuffer> mPerTerrainCBuffer;
		DX11CPUDynamicBuffer mTransformsBuffer;
		DX11Mesh mQuadMesh;
		DX11VertexTransformPass& mVertexTransformer;

		RenderSettings::Tesselation mTessData;
		// row-major layout
		std::array<Mat4, GRID_SIZE> mPatchTranslationTransforms;
		std::array<Mat4, GRID_SIZE> mTempPatchTransformBuffer;
	};
}
#pragma once

#include "Renderer/DirectX11/DX11Utils.h"
#include "Renderer/DirectX11/DX11CPUDynamicBuffer.h"
#include "Renderer/DirectX11/DX11Texture.h"
#include "Renderer/DirectX11/DX11Mesh.h"
#include "Renderer/DirectX11/DX11GBuffer.h"
#include "Renderer/DirectX11/DX11LightAccumulationbuffer.h"
#include "Renderer/DirectX11/DX11Backbuffer.h"
#include "Renderer/DirectX11/DX11AABBPass.h"
#include "Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "Renderer/DirectX11/DX11VertexTransformPass.h"
#include "Renderer/DirectX11/DX11AmbientPass.h"
#include "Renderer/DirectX11/DX11DirectionalLightPass.h"
#include "Renderer/DirectX11/DX11PointLightPass.h"
#include "Renderer/DirectX11/DX11ToneMapper.h"
#include "Renderer/DirectX11/DX11SkyboxPass.h"
#include "Renderer/DirectX11/DX11TerrainPass.h"
#include "Renderer/DirectX11/DX11PostProcessor.h"
#include "Core/DebugOptions.h"
#include "Core/EngineSettings.h"
#include "Core/Types.h"
#include "Core/Containers/IDMap.hpp"
#include "Core/Utils/TimeUtils.h"
#include "Core/Platform.h"

namespace JonsEngine
{
	struct RenderSettings;
    struct RenderQueue;

    class DX11Pipeline
    {
    public:
        DX11Pipeline(Logger& logger, ID3D11DevicePtr device, IDXGISwapChainPtr swapchain, ID3D11DeviceContextPtr context, D3D11_TEXTURE2D_DESC backbufferTextureDesc, DX11Backbuffer& backbuffer,
			const RenderSettings& settings, IDMap<DX11Mesh>& meshMap, IDMap<DX11Texture>& textureMap);
		~DX11Pipeline() = default;

        void BeginFrame( const RenderQueue& renderQueue, const RenderSettings& renderSettings );
        void EndFrame( const RenderSettings& renderSettings );

        void GeometryStage( const RenderQueue& renderQueue, const RenderSettings& renderSettings );
        void LightingStage( const RenderQueue& renderQueue, const RenderSettings& renderSettings );
        void PostProcessingStage(const RenderQueue& renderQueue, const Milliseconds elapstedFrameTime, const RenderSettings& renderSettings);
		void DebugStage( const RenderQueue& renderQueue, const RenderSettings& renderSettings, const DebugOptions::RenderingFlags debugFlags );

    private:
		struct PerFrameCB
		{
			Mat4 mViewProj;
			Mat4 mView;
			Mat4 mInvView;
			Mat4 mInvProj;
			Vec3 mWorldEyePos;
			float __padding1;
			Vec2 mMinMaxZ;
			Vec2 mScreenSize;
			float mTerrainMinSize;
			float mTerrainMaxSize;
			float mTerrainMeshSize;
			float __padding2[ 1 ];

			PerFrameCB() {}
			PerFrameCB( const Mat4& viewProj, const Mat4& view, const Mat4& invView, const Mat4& invProj, const Vec3& worldEyePos, const Vec2& minMaxZ, const Vec2& screenSize,
				float terrainMinSize, float terrainMaxSize, float terrainMeshSize ) :
				mViewProj(viewProj),
				mView(view),
				mInvView(invView),
				mInvProj(invProj),
				mWorldEyePos( worldEyePos ),
				mMinMaxZ( minMaxZ ),
				mScreenSize( screenSize ),
				mTerrainMinSize( terrainMinSize ),
				mTerrainMaxSize( terrainMaxSize ),
				mTerrainMeshSize( terrainMeshSize )
			{
			}
		};

		void RenderMeshes(const RenderQueue& renderQueue, const RenderableMesh::ContainerType& meshContainer, const RenderableMesh::Index begin, const RenderableMesh::Index end);
		void PerformTonemapping(const Milliseconds elapstedFrameTime, const RenderSettings::AutoExposureRate exposureRate, const RenderSettings::ToneMappingAlghorithm alghorithm);
		void SetPerFrameCBuffer( const RenderQueue& renderQueue, const RenderSettings& renderSettings );


        Logger& mLogger;
        const Vec2 mWindowSize;
        IDMap<DX11Mesh>& mMeshMap;
        IDMap<DX11Texture>& mTextureMap;

        IDXGISwapChainPtr mSwapchain;
        ID3D11DeviceContextPtr mContext;
        ID3D11DepthStencilViewPtr mDSV;
        ID3D11DepthStencilViewPtr mDSVReadOnly;
        ID3D11ShaderResourceViewPtr mDepthSRV;
        ID3D11DepthStencilStatePtr mDepthStencilState;
        ID3D11BlendStatePtr mAdditiveBlending;
		ID3D11PixelShaderPtr mSimpleTextureShader;
    
        DX11CPUDynamicBuffer mBoneTransformsBuffer;
        DX11VertexTransformPass mVertexTransformPass;
        DX11AABBPass mAABBPass;
        DX11FullscreenTrianglePass mFullscreenPass;

        DX11Backbuffer& mBackbuffer;
        DX11LightAccumulationbuffer mLightAccbuffer;
        DX11GBuffer mGBuffer;

        DX11AmbientPass mAmbientPass;
        DX11DirectionalLightPass mDirectionalLightPass;
        DX11PointLightPass mPointLightPass;
		DX11ToneMapper mToneMapper;
        DX11PostProcessor mPostProcessor;
        DX11SkyboxPass mSkyboxPass;

		DX11TerrainPass mTerrainPass;

		DX11ConstantBuffer<PerFrameCB> mPerFrameCB;
    };
}

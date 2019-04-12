#pragma once

#include "DX11Utils.h"
#include "DX11CPUDynamicBuffer.h"
#include "DX11Texture.h"
#include "DX11Mesh.h"
#include "DX11GBuffer.h"
#include "DX11LightAccumulationbuffer.h"
#include "DX11Backbuffer.h"
#include "DX11AABBPass.h"
#include "DX11FullscreenTrianglePass.h"
#include "DX11VertexTransformPass.h"
#include "DX11AmbientPass.h"
#include "DX11DirectionalLightPass.h"
#include "DX11PointLightPass.h"
#include "DX11ToneMapper.h"
#include "DX11SkyboxPass.h"
#include "DX11TerrainPass.h"
#include "DX11PostProcessor.h"
#include "DebugOptions.h"
#include "EngineSettings.h"
#include "Types.h"
#include "IDMap.hpp"
#include "TimeUtils.h"
#include "Platform.h"

namespace JonsEngine
{
	struct RenderSettings;
    struct RenderQueue;

    class DX11Pipeline
    {
    public:
        DX11Pipeline(Logger& logger, ID3D11DevicePtr device, IDXGISwapChainPtr swapchain, ID3D11DeviceContextPtr context, D3D11_TEXTURE2D_DESC backbufferTextureDesc, DX11Backbuffer& backbuffer,
			const RenderSettings& settings, IDMap<DX11Mesh>& meshMap, IDMap<DX11Texture>& textureMap);
        ~DX11Pipeline();

        void BeginFrame(const RenderQueue& renderQueue);
        void EndFrame();

        void GeometryStage(const RenderQueue& renderQueue);
        void LightingStage(const RenderQueue& renderQueue, const RenderSettings& renderSettings);
        void PostProcessingStage(const RenderQueue& renderQueue, const Milliseconds elapstedFrameTime, const RenderSettings& renderSettings);
		void DebugStage( const RenderQueue& renderQueue, const DebugOptions::RenderingFlags debugFlags );

    private:
		struct PerFrameCB
		{
			Mat4 mViewProj;
			Mat4 mView;
			Mat4 mInvView;
			Mat4 mInvProj;
			Vec3 mWorldEyePos;
			float mMinZ;
			float mMaxZ;
			float __padding[ 3 ];

			PerFrameCB() {}
			PerFrameCB( const Mat4& viewProj, const Mat4& view, const Mat4& invView, const Mat4& invProj, const Vec3& worldEyePos, float minZ, float maxZ ) :
				mViewProj(viewProj),
				mView(view),
				mInvView(invView),
				mInvProj(invProj),
				mWorldEyePos( worldEyePos ),
				mMinZ( minZ ),
				mMaxZ( maxZ )
			{
			}
		};

		void RenderMeshes(const RenderQueue& renderQueue, const RenderableMesh::ContainerType& meshContainer, const RenderableMesh::Index begin, const RenderableMesh::Index end);
		void PerformTonemapping(const Milliseconds elapstedFrameTime, const RenderSettings::AutoExposureRate exposureRate, const RenderSettings::ToneMappingAlghorithm alghorithm);
		void SetPerFrameCBuffer(const RenderQueue& renderQueue);


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

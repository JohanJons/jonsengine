#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11DynamicBuffer.h"
#include "include/Renderer/DirectX11/DX11Material.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11GBuffer.h"
#include "include/Renderer/DirectX11/DX11LightAccumulationbuffer.h"
#include "include/Renderer/DirectX11/DX11Backbuffer.h"
#include "include/Renderer/DirectX11/DX11AABBPass.h"
#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"
#include "include/Renderer/DirectX11/DX11AmbientPass.h"
#include "include/Renderer/DirectX11/DX11DirectionalLightPass.h"
#include "include/Renderer/DirectX11/DX11PointLightPass.h"
#include "include/Renderer/DirectX11/DX11ToneMapper.h"
#include "include/Renderer/DirectX11/DX11SkyboxPass.h"
#include "include/Renderer/DirectX11/DX11PostProcessor.h"
#include "include/Core/DebugOptions.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"
#include "include/Core/Platform/Directx11.h"
#include "include/RenderQueue/RenderableMesh.h"

namespace JonsEngine
{
    struct RenderQueue;

    class DX11Pipeline
    {
    public:
        DX11Pipeline(Logger& logger, ID3D11DevicePtr device, IDXGISwapChainPtr swapchain, ID3D11DeviceContextPtr context, D3D11_TEXTURE2D_DESC backbufferTextureDesc, const EngineSettings::ShadowResolution shadowmapResolution,
            const EngineSettings::ShadowReadbackLatency shadowmapReadbackLatency, IDMap<DX11Mesh>& meshMap, IDMap<DX11Material>& materialMap);
        ~DX11Pipeline();

        void BeginFrame(const RenderQueue& renderQueue);
        void EndFrame();

        void GeometryStage(const RenderQueue& renderQueue);
        void LightingStage(const RenderQueue& renderQueue, const DebugOptions::RenderingFlags debugFlags, const EngineSettings::ShadowFiltering shadowFiltering, const bool SSAOEnabled);
        void PostProcessingStage(const RenderQueue& renderQueue, const DebugOptions::RenderingFlags debugFlags, const EngineSettings::AntiAliasing AA);


    private:
		void RenderMeshes(const RenderQueue& renderQueue, const RenderableMesh::ContainerType& meshContainer, const RenderableMesh::Index begin, const RenderableMesh::Index end);


        Logger& mLogger;
        const Vec2 mWindowSize;
        IDMap<DX11Mesh>& mMeshMap;
        IDMap<DX11Material>& mMaterialMap;

        IDXGISwapChainPtr mSwapchain;
        ID3D11DeviceContextPtr mContext;
        ID3D11Texture2DPtr mDepthStencilBuffer;
        ID3D11DepthStencilViewPtr mDSV;
        ID3D11DepthStencilViewPtr mDSVReadOnly;
        ID3D11ShaderResourceViewPtr mDepthSRV;
        ID3D11DepthStencilStatePtr mDepthStencilState;
        ID3D11BlendStatePtr mAdditiveBlending;
    
        DX11DynamicBuffer mBoneTransformsBuffer;
        DX11VertexTransformPass mVertexTransformPass;
        DX11AABBPass mAABBPass;
        DX11FullscreenTrianglePass mFullscreenTrianglePass;

        DX11Backbuffer mBackbuffer;
        DX11LightAccumulationbuffer mLightAccbuffer;
        DX11GBuffer mGBuffer;

        DX11AmbientPass mAmbientPass;
        DX11DirectionalLightPass mDirectionalLightPass;
        DX11PointLightPass mPointLightPass;
		DX11ToneMapper mToneMapper;
        DX11PostProcessor mPostProcessor;
        DX11SkyboxPass mSkyboxPass;
    };
}

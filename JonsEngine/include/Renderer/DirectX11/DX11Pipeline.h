#pragma once

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
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
#include "include/Renderer/DirectX11/DX11PostProcessor.h"
#include "include/Core/DebugOptions.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.h"

#include <d3d11.h>

namespace JonsEngine
{
    class DX11Pipeline
    {
    public:
        DX11Pipeline(Logger& logger, ID3D11DevicePtr device, IDXGISwapChainPtr swapchain, ID3D11DeviceContextPtr context, D3D11_TEXTURE2D_DESC backbufferTextureDesc, const EngineSettings::ShadowResolution shadowmapResolution,
            const EngineSettings::ShadowReadbackLatency shadowmapReadbackLatency, IDMap<DX11Mesh>& meshMap, IDMap<DX11Texture>& textureMap);
        ~DX11Pipeline();

        void BeginFrame();
        void EndFrame();

        void GeometryStage(const RenderQueue& renderQueue);
        void LightingStage(const RenderQueue& renderQueue, const DebugOptions::RenderingFlags debugFlags, const EngineSettings::ShadowFiltering shadowFiltering, const bool SSAOEnabled);
        void PostProcessingStage(const RenderQueue& renderQueue, const DebugOptions::RenderingFlags debugFlags, const EngineSettings::AntiAliasing AA);


    private:
        Logger& mLogger;
        const Vec2 mWindowSize;
        IDMap<DX11Mesh>& mMeshMap;
        IDMap<DX11Texture>& mTextureMap;

        IDXGISwapChainPtr mSwapchain;
        ID3D11DeviceContextPtr mContext;
        ID3D11Texture2DPtr mDepthStencilBuffer;
        ID3D11DepthStencilViewPtr mDSV;
        ID3D11DepthStencilViewPtr mDSVReadOnly;
        ID3D11ShaderResourceViewPtr mDepthSRV;
        ID3D11DepthStencilStatePtr mDepthStencilState;
        ID3D11BlendStatePtr mAdditiveBlending;
    
        DX11VertexTransformPass mVertexTransformPass;
        DX11AABBPass mAABBPass;
        DX11FullscreenTrianglePass mFullscreenTrianglePass;

        DX11Backbuffer mBackbuffer;
        DX11LightAccumulationbuffer mLightAccbuffer;
        DX11GBuffer mGBuffer;

        DX11AmbientPass mAmbientPass;
        DX11DirectionalLightPass mDirectionalLightPass;
        DX11PointLightPass mPointLightPass;
        DX11PostProcessor mPostProcessor;
    };
}

#pragma once

#include "DX11Mesh.h"
#include "DX11Texture.h"
#include "DX11Context.h"
#include "DX11Pipeline.h"
#include "DX11Backbuffer.h"
#include "DX11DepthBufferReadback.h"
#include "DX11DepthReductionPass.h"
#include "DX11Sampler.h"
#include "DX11Utils.h"
#include "Types.h"
#include "Types.h"
#include "EngineSettings.h"
#include "IDMap.hpp"
#include "DebugOptions.h"
#include "Time.h"
#include "Directx11.h"
#include "Bone.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>


namespace JonsEngine
{
    class Logger;
    class HeapAllocator;
    struct RenderQueue;
	struct RenderSettings;
	struct WindowSettings;

    class DX11RendererImpl : protected DX11Context
    {
    public:
        DX11RendererImpl(const RenderSettings& renderSettings, const WindowSettings& windowSettings, Logger& logger, HeapAllocator& memoryAllocator);
        ~DX11RendererImpl();

        DX11MeshID CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangentData,
            const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds);
        DX11MeshID CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangentData,
            const std::vector<BoneWeight>& boneWeights, const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds);
        DX11TextureID CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight);

        void Render(const RenderQueue& renderQueue, const Milliseconds elapstedFrameTime, const DebugOptions::RenderingFlags debugFlags);
        void ReduceDepth(float& minDepth, float& maxDepth);

        RenderSettings::Anisotropic GetAnisotropicFiltering() const;
        void SetAnisotropicFiltering(const RenderSettings::Anisotropic anisotropic);

		RenderSettings::AntiAliasing GetAntiAliasing() const;
        void SetAntiAliasing(const RenderSettings::AntiAliasing aa);

		RenderSettings::ShadowFiltering GetShadowFiltering() const;
		void SetShadowFiltering(const RenderSettings::ShadowFiltering shadowFiltering);

		RenderSettings::ToneMappingAlghorithm GetToneMappingAlghorithm() const;
		void SetToneMappingAlghorithm(const RenderSettings::ToneMappingAlghorithm alghorithm);

        bool IsSSAOEnabled() const;
        void SetSSAO(const bool useSSAO);

		float GetDepthValue(const WindowPosition& position);
        float GetZNear() const;
        float GetZFar() const;
		RenderSettings::ShadowResolution GetShadowResolution() const;
		RenderSettings::ShadowReadbackLatency GetShadowReadbackLatency() const;


    private:
        static LRESULT CALLBACK DX11RendererImpl::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
        void SetupContext(const uint32_t viewportWidth, const uint32_t viewportHeight);

        Logger& mLogger;
        HeapAllocator& mMemoryAllocator;
        IDMap<DX11Mesh> mMeshes;
        IDMap<DX11Texture> mTextures;

		DX11Backbuffer mBackbuffer;

		RenderSettings mRenderSettings;
		DX11DepthBufferReadback mDepthReadback;
        DX11DepthReductionPass mDepthReductionPass;
        ID3D11RasterizerStatePtr mDefaultRasterizerState;
        std::unique_ptr<DX11Sampler, std::function<void(DX11Sampler*)>> mModelSampler;
        DX11Sampler mLinearSampler;
		DX11Sampler mLinearWrapSampler;
        DX11Sampler mShadowmapSampler;
        DX11Sampler mShadowmapNoCompareSampler;
        bool mSSAOEnabled;

		DX11Pipeline mPipeline;
    };
}
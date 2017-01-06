#pragma once

#include "include/Core/Types.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/DebugOptions.h"
#include "include/Core/Utils/Time.h"
#include "include/Resources/Bone.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>


namespace JonsEngine
{
    class Logger;
    class HeapAllocator;
    class DX11RendererImpl;
	struct RenderSettings;
	struct WindowSettings;
	struct RenderQueue;

    class DX11Renderer
    {
    public:
        DX11Renderer(const RenderSettings& renderSettings, const WindowSettings& windowSettings, HeapAllocator& memoryAllocator, Logger& logger);
        ~DX11Renderer();

        DX11MeshID CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangentData,
            const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds);
        DX11MeshID CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangentData,
            const std::vector<BoneWeight>& boneWeights, const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds);
        DX11MaterialID CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight);

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

        float GetZNear() const;
        float GetZFar() const;
		RenderSettings::ShadowResolution GetShadowResolution() const;
		RenderSettings::ShadowReadbackLatency GetShadowReadbackLatency() const;


    private:
        HeapAllocator& mMemoryAllocator;
        std::unique_ptr<DX11RendererImpl, std::function<void(DX11RendererImpl*)>> mImplementation;
    };
}
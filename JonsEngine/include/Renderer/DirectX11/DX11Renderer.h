#pragma once

#include "include/Renderer/RenderQueue.h"
#include "include/Core/Types.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/DebugOptions.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>


namespace JonsEngine
{
    class Logger;
    class HeapAllocator;
    class DX11RendererImpl;

    class DX11Renderer
    {
    public:
        DX11Renderer(const EngineSettings& settings, HeapAllocator& memoryAllocator, Logger& logger);
        ~DX11Renderer();

        DX11MeshID CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangentData,
            const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds);
        DX11MaterialID CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight);

        void Render(const RenderQueue& renderQueue, const DebugOptions::RenderingFlags debugFlags);
        void ReduceDepth(float& minDepth, float& maxDepth);

        EngineSettings::Anisotropic GetAnisotropicFiltering() const;
        void SetAnisotropicFiltering(const EngineSettings::Anisotropic anisotropic);

        EngineSettings::AntiAliasing GetAntiAliasing() const;
        void SetAntiAliasing(const EngineSettings::AntiAliasing aa);

		EngineSettings::ShadowFiltering GetShadowFiltering() const;
		void SetShadowFiltering(const EngineSettings::ShadowFiltering shadowFiltering);

        bool IsSSAOEnabled() const;
        void SetSSAO(const bool useSSAO);

        float GetZNear() const;
        float GetZFar() const;
        EngineSettings::ShadowResolution GetShadowResolution() const;
        EngineSettings::ShadowReadbackLatency GetShadowReadbackLatency() const;


    private:
        HeapAllocator& mMemoryAllocator;
        std::unique_ptr<DX11RendererImpl, std::function<void(DX11RendererImpl*)>> mImplementation;
    };
}
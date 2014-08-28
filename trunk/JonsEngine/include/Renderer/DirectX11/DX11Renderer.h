#pragma once

#include "include/Renderer/RenderCommands.h"
#include "include/Core/Types.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/Memory/IMemoryAllocator.h"
#include "include/Core/DebugOptions.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>


namespace JonsEngine
{
    class Logger;
    class DX11RendererImpl;

    class DX11Renderer
    {
    public:
        DX11Renderer(const EngineSettings& settings, IMemoryAllocatorPtr memoryAllocator, Logger& logger);
        ~DX11Renderer();

        MeshID CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData);
        TextureID CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight);

        void Render(const RenderQueue& renderQueue, const RenderableLighting& lighting, const DebugOptions::RenderingMode debugMode, const DebugOptions::RenderingFlags debugExtra);

        EngineSettings::Anisotropic GetAnisotropicFiltering() const;
        void SetAnisotropicFiltering(const EngineSettings::Anisotropic anisotropic);

        EngineSettings::MSAA GetMSAA() const;
        void SetMSAA(const EngineSettings::MSAA msaa);

        float GetZNear() const;
        float GetZFar() const;
        uint32_t GetShadowmapResolution() const;


    private:
        IMemoryAllocatorPtr mMemoryAllocator;
        std::unique_ptr<DX11RendererImpl, std::function<void(DX11RendererImpl*)>> mImplementation;
    };
}
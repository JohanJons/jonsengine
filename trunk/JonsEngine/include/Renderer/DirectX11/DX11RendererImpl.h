#pragma once

#include "include/Renderer/RenderQueue.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11Context.h"
#include "include/Renderer/DirectX11/DX11Pipeline.h"
#include "include/Renderer/DirectX11/DX11GBuffer.h"
#include "include/Renderer/DirectX11/DX11Sampler.h"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Core/Types.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/Containers/IDMap.h"
#include "include/Core/DebugOptions.h"

#include <windows.h>
#include <d3d11.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>


namespace JonsEngine
{
    class Logger;

    class DX11RendererImpl : protected DX11Context
    {
    public:
        DX11RendererImpl(const EngineSettings& settings, Logger& logger, IMemoryAllocatorPtr memoryAllocator);
        ~DX11RendererImpl();

        MeshID CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents,
            const std::vector<float>& bitangents, const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds);
        TextureID CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight);

        void Render(const RenderQueue& renderQueue, const DebugOptions::RenderingFlags debugFlags);

        EngineSettings::Anisotropic GetAnisotropicFiltering() const;
        void SetAnisotropicFiltering(const EngineSettings::Anisotropic anisotropic);

        EngineSettings::AntiAliasing GetAntiAliasing() const;
        void SetAntiAliasing(const EngineSettings::AntiAliasing aa);

        bool IsSSAOEnabled() const;
        void SetSSAO(const bool useSSAO);

        float GetZNear() const;
        float GetZFar() const;
        EngineSettings::ShadowResolution GetShadowResolution() const;
        EngineSettings::ShadowReadbackLatency GetShadowReadbackLatency() const;


    private:
        static LRESULT CALLBACK DX11RendererImpl::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
        void SetupContext(const uint32_t viewportWidth, const uint32_t viewportHeight);

        Logger& mLogger;
        IMemoryAllocatorPtr mMemoryAllocator;
        IDMap<DX11Mesh> mMeshes;
        IDMap<DX11Texture> mTextures;

        EngineSettings::ShadowResolution mShadowResolution;
        EngineSettings::ShadowReadbackLatency mShadowReadbackLatency;
        EngineSettings::AntiAliasing mAntiAliasing;

        DX11Pipeline mPipeline;
        ID3D11RasterizerStatePtr mDefaultRasterizerState;
        std::unique_ptr<DX11Sampler, std::function<void(DX11Sampler*)>> mModelSampler;
        DX11Sampler mShadowmapSampler;
        DX11Sampler mShadowmapNoCompareSampler;
        bool mSSAOEnabled;
    };
}
#pragma once

#include "include/Renderer/RenderCommands.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11Context.h"
#include "include/Renderer/DirectX11/DX11BackBuffer.h"
#include "include/Renderer/DirectX11/DX11GBuffer.h"
#include "include/Renderer/DirectX11/DX11AmbientPass.h"
#include "include/Renderer/DirectX11/DX11DirectionalLightPass.h"
#include "include/Renderer/DirectX11/DX11PointLightPass.h"
#include "include/Core/Types.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/Memory/HeapAllocator.h"
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
        typedef std::shared_ptr<DX11Mesh> DX11MeshPtr;
        typedef std::shared_ptr<DX11Texture> DX11TexturePtr;


        DX11RendererImpl(const EngineSettings& settings, Logger& logger, IMemoryAllocatorPtr memoryAllocator);
        ~DX11RendererImpl();

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
        static LRESULT CALLBACK DX11RendererImpl::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
        void SetupContext(const uint32_t viewportWidth, const uint32_t viewportHeight);
        void GeometryStage(const RenderQueue& renderQueue);
        void ShadingStage(const RenderQueue& renderQueue, const RenderableLighting& lighting);

        Logger& mLogger;
        IMemoryAllocatorPtr mMemoryAllocator;
        EngineSettings::Anisotropic mAnisotropicFiltering;
        EngineSettings::ShadowQuality mShadowQuality;

        DX11BackBuffer mBackBuffer;
        DX11GBuffer mGBuffer;
        DX11AmbientPass mAmbientPass;
        DX11DirectionalLightPass mDirectionalLightPass;
        DX11PointLightPass mPointLightPass;
        ID3D11RasterizerState* mDefaultRasterizerState;
        ID3D11BlendState* mBlendState;
        ID3D11SamplerState* mTextureSampler;

        std::vector<DX11MeshPtr> mMeshes;
        std::vector<DX11TexturePtr> mTextures;
    };
}
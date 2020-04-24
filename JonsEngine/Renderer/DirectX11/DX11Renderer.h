#pragma once

#include "Renderer/DirectX11/DX11Mesh.h"
#include "Renderer/DirectX11/DX11Texture.h"
#include "Renderer/DirectX11/DX11Context.h"
#include "Renderer/DirectX11/DX11Pipeline.h"
#include "Renderer/DirectX11/DX11Backbuffer.h"
#include "Renderer/DirectX11/DX11DepthBufferReadback.h"
#include "Renderer/DirectX11/DX11DepthReductionPass.h"
#include "Renderer/DirectX11/DX11Sampler.h"
#include "Renderer/DirectX11/DX11Utils.h"
#include "Core/Types.h"
#include "Core/Types.h"
#include "Core/EngineSettings.h"
#include "Core/Containers/IDMap.hpp"
#include "Core/DebugOptions.h"
#include "Core/Utils/TimeUtils.h"
#include "Core/Platform.h"
#include "Resources/Bone.h"

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

    class DX11Renderer : protected DX11Context
    {
    public:
        DX11Renderer(const RenderSettings& renderSettings, const WindowSettings& windowSettings, HeapAllocator& memoryAllocator, Logger& logger);
        ~DX11Renderer();

        DX11MeshID CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangentData,
            const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds);
        DX11MeshID CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangentData,
            const std::vector<BoneWeight>& boneWeights, const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds);
        DX11TextureID CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight);

        void Render(const RenderQueue& renderQueue, const Milliseconds elapstedFrameTime, const DebugOptions::RenderingFlags debugFlags);
        void ReduceDepth( float& minDepth, float& maxDepth );

        RenderSettings::Anisotropic GetAnisotropicFiltering() const;
        void SetAnisotropicFiltering(const RenderSettings::Anisotropic anisotropic);

        RenderSettings::AntiAliasing GetAntiAliasing() const;
        void SetAntiAliasing(const RenderSettings::AntiAliasing aa);

        RenderSettings::ShadowFiltering GetShadowFiltering() const;
        void SetShadowFiltering(const RenderSettings::ShadowFiltering shadowFiltering);

        RenderSettings::ToneMappingAlghorithm GetToneMappingAlghorithm() const;
        void SetToneMappingAlghorithm(const RenderSettings::ToneMappingAlghorithm alghorithm);

		RenderSettings::TerrainPatchMinSize GetTerrainPatchMinSize() const { return mRenderSettings.mTerrainPatchMinSize; }
		void SetTerrainPatchMinSize( RenderSettings::TerrainPatchMinSize patchSize );

		RenderSettings::TerrainPatchMaxSize GetTerrainPatchMaxSize() const { return mRenderSettings.mTerrainPatchMaxSize; }
		void SetTerrainPatchMaxSize( RenderSettings::TerrainPatchMaxSize patchSize );

		RenderSettings::TerrainMeshDimensions GetTerrainPatchVertexRatio() const { return mRenderSettings.mTerrainMeshDimensions; }
		void SetTerrainMeshDimensions( RenderSettings::TerrainMeshDimensions meshDimensions );

		RenderSettings::TerrainNormals GetTerrainNormals() const { return mRenderSettings.mTerrainNormals; }
		void SetTerrainNormals( RenderSettings::TerrainNormals normals ) { mRenderSettings.mTerrainNormals = normals; }

        bool IsSSAOEnabled() const;
        void SetSSAO(const bool useSSAO);

        float GetDepthValue(const WindowPosition& position);

        float GetZNear() const;
        float GetZFar() const;
		void SetZNear( float zNear );
		void SetZFar( float zFar );

        RenderSettings::ShadowResolution GetShadowResolution() const;
        RenderSettings::ShadowReadbackLatency GetShadowReadbackLatency() const;


    private:
        static LRESULT CALLBACK DX11Renderer::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
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

        DX11Pipeline mPipeline;
    };
}
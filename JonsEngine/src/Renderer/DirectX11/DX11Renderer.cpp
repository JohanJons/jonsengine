#include "include/Renderer/DirectX11/DX11Renderer.h"

#include "include/Renderer/DirectX11/DX11RendererImpl.h"
#include "include/Core/Memory/HeapAllocator.h"


namespace JonsEngine
{
    DX11Renderer::DX11Renderer(const EngineSettings& settings, HeapAllocator& memoryAllocator, Logger& logger) :
        mMemoryAllocator(memoryAllocator), mImplementation(mMemoryAllocator.AllocateObject<DX11RendererImpl>(settings, logger, memoryAllocator), [this](DX11RendererImpl* directxImpl) { mMemoryAllocator.DeallocateObject(directxImpl); })
    {
    }

    DX11Renderer::~DX11Renderer()
    {
    }


    DX11MeshID DX11Renderer::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangentData,
        const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds)
    {
        return mImplementation->CreateMesh(vertexData, normalData, texCoords, tangentData, indexData, minBounds, maxBounds);
    }

    DX11MeshID DX11Renderer::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangentData,
		const std::vector<BoneWeight>& boneWeights, const std::vector<uint16_t>& indexData, const Vec3& minBounds, const Vec3& maxBounds)
    {
        return mImplementation->CreateMesh(vertexData, normalData, texCoords, tangentData, boneWeights, indexData, minBounds, maxBounds);
    }

    DX11MaterialID DX11Renderer::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight)
    {
        return mImplementation->CreateTexture(textureType, textureData, textureWidth, textureHeight);
    }


    void DX11Renderer::Render(const RenderQueue& renderQueue, const DebugOptions::RenderingFlags debugFlags)
    {
        mImplementation->Render(renderQueue, debugFlags);
    }

    void DX11Renderer::ReduceDepth(float& minDepth, float& maxDepth)
    {
        mImplementation->ReduceDepth(minDepth, maxDepth);
    }


    EngineSettings::Anisotropic DX11Renderer::GetAnisotropicFiltering() const
    {
        return mImplementation->GetAnisotropicFiltering();
    }

    void DX11Renderer::SetAnisotropicFiltering(const EngineSettings::Anisotropic anisotropic)
    {
        mImplementation->SetAnisotropicFiltering(anisotropic);
    }


    EngineSettings::AntiAliasing DX11Renderer::GetAntiAliasing() const
    {
        return mImplementation->GetAntiAliasing();
    }

    void DX11Renderer::SetAntiAliasing(const EngineSettings::AntiAliasing aa)
    {
        mImplementation->SetAntiAliasing(aa);
    }


	EngineSettings::ShadowFiltering DX11Renderer::GetShadowFiltering() const
	{
		return mImplementation->GetShadowFiltering();
	}

	void DX11Renderer::SetShadowFiltering(const EngineSettings::ShadowFiltering shadowFiltering)
	{
		mImplementation->SetShadowFiltering(shadowFiltering);
	}


    bool DX11Renderer::IsSSAOEnabled() const
    {
        return mImplementation->IsSSAOEnabled();
    }

    void DX11Renderer::SetSSAO(const bool useSSAO)
    {
        mImplementation->SetSSAO(useSSAO);
    }


    float DX11Renderer::GetZNear() const
    {
        return mImplementation->GetZNear();
    }

    float DX11Renderer::GetZFar() const
    {
        return mImplementation->GetZFar();
    }

    EngineSettings::ShadowResolution DX11Renderer::GetShadowResolution() const
    {
        return mImplementation->GetShadowResolution();
    }

    EngineSettings::ShadowReadbackLatency DX11Renderer::GetShadowReadbackLatency() const
    {
        return mImplementation->GetShadowReadbackLatency();
    }
}
#include "DX11Renderer.h"

#include "DX11RendererImpl.h"
#include "HeapAllocator.h"


namespace JonsEngine
{
    DX11Renderer::DX11Renderer(const RenderSettings& renderSettings, const WindowSettings& windowSettings, HeapAllocator& memoryAllocator, Logger& logger) :
        mMemoryAllocator(memoryAllocator), mImplementation(mMemoryAllocator.AllocateObject<DX11RendererImpl>(renderSettings, windowSettings, logger, memoryAllocator), [this](DX11RendererImpl* directxImpl) { mMemoryAllocator.DeallocateObject(directxImpl); })
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

    DX11TextureID DX11Renderer::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight)
    {
        return mImplementation->CreateTexture(textureType, textureData, textureWidth, textureHeight);
    }


    void DX11Renderer::Render(const RenderQueue& renderQueue, const Milliseconds elapstedFrameTime, const DebugOptions::RenderingFlags debugFlags)
    {
        mImplementation->Render(renderQueue, elapstedFrameTime, debugFlags);
    }

    void DX11Renderer::ReduceDepth(float& minDepth, float& maxDepth)
    {
        mImplementation->ReduceDepth(minDepth, maxDepth);
    }


	RenderSettings::Anisotropic DX11Renderer::GetAnisotropicFiltering() const
    {
        return mImplementation->GetAnisotropicFiltering();
    }

    void DX11Renderer::SetAnisotropicFiltering(const RenderSettings::Anisotropic anisotropic)
    {
        mImplementation->SetAnisotropicFiltering(anisotropic);
    }


	RenderSettings::AntiAliasing DX11Renderer::GetAntiAliasing() const
    {
        return mImplementation->GetAntiAliasing();
    }

    void DX11Renderer::SetAntiAliasing(const RenderSettings::AntiAliasing aa)
    {
        mImplementation->SetAntiAliasing(aa);
    }


	RenderSettings::ShadowFiltering DX11Renderer::GetShadowFiltering() const
	{
		return mImplementation->GetShadowFiltering();
	}

	void DX11Renderer::SetShadowFiltering(const RenderSettings::ShadowFiltering shadowFiltering)
	{
		mImplementation->SetShadowFiltering(shadowFiltering);
	}


	RenderSettings::ToneMappingAlghorithm DX11Renderer::GetToneMappingAlghorithm() const
	{
		return mImplementation->GetToneMappingAlghorithm();
	}

	void DX11Renderer::SetToneMappingAlghorithm(const RenderSettings::ToneMappingAlghorithm alghorithm)
	{
		mImplementation->SetToneMappingAlghorithm(alghorithm);
	}


    bool DX11Renderer::IsSSAOEnabled() const
    {
        return mImplementation->IsSSAOEnabled();
    }

    void DX11Renderer::SetSSAO(const bool useSSAO)
    {
        mImplementation->SetSSAO(useSSAO);
    }


	float DX11Renderer::GetDepthValue(const WindowPosition& position)
	{
		return mImplementation->GetDepthValue(position);
	}

    float DX11Renderer::GetZNear() const
    {
        return mImplementation->GetZNear();
    }

    float DX11Renderer::GetZFar() const
    {
        return mImplementation->GetZFar();
    }

	RenderSettings::ShadowResolution DX11Renderer::GetShadowResolution() const
    {
        return mImplementation->GetShadowResolution();
    }

	RenderSettings::ShadowReadbackLatency DX11Renderer::GetShadowReadbackLatency() const
    {
        return mImplementation->GetShadowReadbackLatency();
    }
}
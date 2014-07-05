#include "include/Renderer/DirectX11/DirectXRenderer.h"

#include "include/Renderer/DirectX11/DirectXRendererImpl.h"


namespace JonsEngine
{
    DirectXRenderer::DirectXRenderer(const EngineSettings& settings, IMemoryAllocatorPtr memoryAllocator, Logger& logger) :
        mMemoryAllocator(memoryAllocator), mImplementation(mMemoryAllocator->AllocateObject<DirectXRendererImpl>(settings, logger), [this](DirectXRendererImpl* directxImpl) { mMemoryAllocator->DeallocateObject(directxImpl); })
    {
    }

    DirectXRenderer::~DirectXRenderer()
    {
    }


    MeshID DirectXRenderer::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData)
    {
        return mImplementation->CreateMesh(vertexData, normalData, texCoords, tangents, bitangents, indexData);
    }

    TextureID DirectXRenderer::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat)
    {
        return mImplementation->CreateTexture(textureType, textureData, textureWidth, textureHeight, colorFormat);
    }


    void DirectXRenderer::Render(const RenderQueue& renderQueue, const RenderableLighting& lighting, const DebugOptions::RenderingMode debugMode, const DebugOptions::RenderingFlags debugExtra)
    {
        mImplementation->Render(renderQueue, lighting, debugMode, debugExtra);
    }


    EngineSettings::Anisotropic DirectXRenderer::GetAnisotropicFiltering() const
    {
        return mImplementation->GetAnisotropicFiltering();
    }

    void DirectXRenderer::SetAnisotropicFiltering(const EngineSettings::Anisotropic anisotropic)
    {
        mImplementation->SetAnisotropicFiltering(anisotropic);
    }


    EngineSettings::MSAA DirectXRenderer::GetMSAA() const
    {
        return mImplementation->GetMSAA();
    }

    void DirectXRenderer::SetMSAA(const EngineSettings::MSAA msaa)
    {
        mImplementation->SetMSAA(msaa);
    }


    float DirectXRenderer::GetZNear() const
    {
        return mImplementation->GetZNear();
    }

    float DirectXRenderer::GetZFar() const
    {
        return mImplementation->GetZFar();
    }

    uint32_t DirectXRenderer::GetShadowmapResolution() const
    {
        return mImplementation->GetShadowmapResolution();
    }
}
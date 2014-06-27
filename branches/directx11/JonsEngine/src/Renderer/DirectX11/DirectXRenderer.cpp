#include "include/Renderer/DirectX11/DirectXRenderer.h"


namespace JonsEngine
{
    DirectXRenderer::DirectXRenderer()
    {

    }

    DirectXRenderer::~DirectXRenderer()
    {

    }


    MeshID DirectXRenderer::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData)
    {
        return INVALID_MESH_ID;
    }
    
    TextureID DirectXRenderer::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat)
    {
        return INVALID_TEXTURE_ID;
    }


    void DirectXRenderer::Render(const RenderQueue& renderQueue, const RenderableLighting& lighting, const DebugOptions::RenderingMode debugMode, const DebugOptions::RenderingFlags debugExtra)
    {

    }


    EngineSettings::Anisotropic DirectXRenderer::GetAnisotropicFiltering() const
    {
        return EngineSettings::ANISOTROPIC_1X;
    }

    void DirectXRenderer::SetAnisotropicFiltering(const EngineSettings::Anisotropic anisotropic)
    {

    }


    EngineSettings::MSAA DirectXRenderer::GetMSAA() const
    {
        return EngineSettings::MSAA_1X;
    }

    void DirectXRenderer::SetMSAA(const EngineSettings::MSAA msaa)
    {

    }


    float DirectXRenderer::GetZNear() const
    {
        return 0.0f;
    }

    float DirectXRenderer::GetZFar() const
    {
        return 1.0f;
    }

    uint32_t DirectXRenderer::GetShadowmapResolution() const
    {
        return 1024;
    }
}
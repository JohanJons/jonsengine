#pragma once

#include "include/Renderer/IRenderer.h"

namespace JonsEngine
{
    /*
     * A default rendering backend that can be used when no proper backend is available during initialization
     */
    
    struct EngineSettings;


    /* DummyRenderBackend definition */
    class DummyRenderer : public IRenderer
    {
    public:
        DummyRenderer(const EngineSettings& engineSettings);
        ~DummyRenderer();

        MeshPtr CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<uint32_t>& indexData);
        TexturePtr CreateTexture(ITexture::TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ITexture::TextureFormat textureFormat);

        void DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting);
        RenderBackendType GetRenderBackendType() const;

        float GetMaxAnisotropyLevel() const;
        float GetCurrentAnisotropyLevel() const;
        bool SetAnisotropyLevel(const float newAnisoLevel);

        float GetZNear() const;
        float GetZFar() const;
    };


    /* DummyRenderBackend inlines */
    inline DummyRenderer::DummyRenderer(const EngineSettings& engineSettings)                                                                                                                                            { }
    inline DummyRenderer::~DummyRenderer()                                                                                                                                                                               { }
   
    inline MeshPtr DummyRenderer::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<uint32_t>& indexData)                    { return MeshPtr(); }
    inline TexturePtr DummyRenderer::CreateTexture(ITexture::TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ITexture::TextureFormat textureFormat)     { return TexturePtr(); }

    inline void DummyRenderer::DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting)                                                                                                       { }
    inline IRenderer::RenderBackendType DummyRenderer::GetRenderBackendType() const                                                                                                                                      { return NONE; }

    inline float DummyRenderer::GetMaxAnisotropyLevel() const                                                                                                                                                            { return 0.0f; }
    inline float DummyRenderer::GetCurrentAnisotropyLevel() const                                                                                                                                                        { return 0.0f; }
    inline bool DummyRenderer::SetAnisotropyLevel(const float newAnisoLevel)                                                                                                                                             { return false; }

    inline float DummyRenderer::GetZNear() const                                                                                                                                                                         { return 0.0f; }
    inline float DummyRenderer::GetZFar() const                                                                                                                                                                          { return 0.0f; }
}
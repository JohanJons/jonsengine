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

        MeshPtr CreateMesh(const std::vector<Vec3>& vertexData, const std::vector<Vec3>& normalData, const std::vector<Vec2>& texCoords, const std::vector<uint32_t>& indexData);
        TexturePtr CreateTexture(ITexture::TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ITexture::TextureFormat textureFormat);

        void DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting);
        RenderBackendType GetRenderBackendType() const;
    };


    /* DummyRenderBackend inlines */
    inline DummyRenderer::DummyRenderer(const EngineSettings& engineSettings)                                                                                                                                            { }
    inline DummyRenderer::~DummyRenderer()                                                                                                                                                                               { }
   
    inline MeshPtr DummyRenderer::CreateMesh(const std::vector<Vec3>& vertexData, const std::vector<Vec3>& normalData, const std::vector<Vec2>& texCoords, const std::vector<uint32_t>& indexData)                       { return MeshPtr(); }
    inline TexturePtr DummyRenderer::CreateTexture(ITexture::TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ITexture::TextureFormat textureFormat)     { return TexturePtr(); }

    inline void DummyRenderer::DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting)                                                                                                       { }
    inline IRenderer::RenderBackendType DummyRenderer::GetRenderBackendType() const                                                                                                                                      { return NONE; }
}
#pragma once

#include "include/Renderer/IRenderer.h"
#include "include/Renderer/IMaterial.h"
#include "include/Renderer/IVertexBuffer.h"

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

        VertexBufferPtr CreateVertexBuffer(const std::vector<Vec3>& vertexData, const std::vector<Vec3>& normalData, const std::vector<Vec2>& texCoords, const std::vector<uint32_t>& indexData);
        MaterialPtr CreateMaterial();
        void DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting);

        RenderBackendType GetRenderBackendType() const;
    };


    /* DummyRenderBackend inlines */
    inline DummyRenderer::DummyRenderer(const EngineSettings& engineSettings)                                                                                                                                               { }
    inline DummyRenderer::~DummyRenderer()                                                                                                                                                                                  { }
    inline void DummyRenderer::DrawRenderables(const RenderQueue& renderQueue, const RenderableLighting& lighting)                                                                                                          { }
    inline VertexBufferPtr DummyRenderer::CreateVertexBuffer(const std::vector<Vec3>& vertexData, const std::vector<Vec3>& normalData, const std::vector<Vec2>& texCoords, const std::vector<uint32_t>& indexData)          { return VertexBufferPtr(); }
    inline MaterialPtr DummyRenderer::CreateMaterial()                                                                                                                                                                      { return MaterialPtr(); }
    inline IRenderer::RenderBackendType DummyRenderer::GetRenderBackendType() const                                                                                                                                         { return NONE; }
}
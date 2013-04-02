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

        VertexBufferPtr CreateVertexBuffer(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<uint32_t>& indexData);
 
        void BeginRendering();
        void DrawRenderables(const std::vector<RenderItem>& renderQueue);
        void EndRendering();

        RenderBackendType GetRenderBackendType() const;
    };


    /* DummyRenderBackend inlines */
    inline IRenderer::RenderBackendType DummyRenderer::GetRenderBackendType() const             { return NONE;  }
}
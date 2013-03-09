#pragma once

#include "include/Renderer/IRenderer.h"
#include "include/Scene/Mesh.h"

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

        VertexBufferPtr CreateVertexBuffer(const std::vector<float>& vertexData, const std::vector<uint32_t>& indexData);
        VertexBufferPtr CreateVertexBuffer(const float vertexData[], const size_t vertexDataSize, const uint32_t indexData[], const size_t indexDataSize);

        void BeginRendering();
        void DrawRenderables(const std::vector<RenderItem>& renderQueue);
        void EndRendering();

        RenderBackendType GetRenderBackendType() const;
    };


    /* DummyRenderBackend inlines */
    inline IRenderer::RenderBackendType DummyRenderer::GetRenderBackendType() const             { return NONE;  }
}
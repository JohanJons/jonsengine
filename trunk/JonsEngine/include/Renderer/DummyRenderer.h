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

        VertexBufferPtr CreateVertexBuffer(const vector<float>& vertexData, const vector<uint16_t>& indexData);
        VertexBufferPtr CreateVertexBuffer(const float vertexData[], const size_t vertexDataSize, const uint16_t indexData[], const size_t indexDataSize);

        void BeginRendering();
        void RenderMesh(MeshPtr mesh);
        void EndRendering();


        RenderBackendType GetRenderBackendType() const;
    };


    /* DummyRenderBackend inlines */
    inline IRenderer::RenderBackendType DummyRenderer::GetRenderBackendType() const             { return NONE;  }
}
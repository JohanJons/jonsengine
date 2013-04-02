#include "include/Renderer/DummyRenderer.h"

namespace JonsEngine
{
    DummyRenderer::DummyRenderer(const EngineSettings& engineSettings)
    {
    }
       
    DummyRenderer::~DummyRenderer()
    {
    }

        
    VertexBufferPtr DummyRenderer::CreateVertexBuffer(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<uint32_t>& indexData)
    {
        return VertexBufferPtr();
    }

      
    void DummyRenderer::BeginRendering()
    {
    }

    void DummyRenderer::DrawRenderables(const std::vector<RenderItem>& renderQueue)
    {
    }

    void DummyRenderer::EndRendering()
    {
    }
}
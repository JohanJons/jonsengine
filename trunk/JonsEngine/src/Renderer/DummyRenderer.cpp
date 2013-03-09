#include "include/Renderer/DummyRenderer.h"

#include "include/Core/EngineDefs.h"

#include <vector>

namespace JonsEngine
{
    DummyRenderer::DummyRenderer(const EngineSettings& engineSettings)
    {
    }
       
    DummyRenderer::~DummyRenderer()
    {
    }

        
    VertexBufferPtr DummyRenderer::CreateVertexBuffer(const std::vector<float>& vertexData, const std::vector<uint32_t>& indexData)
    {
        return VertexBufferPtr();
    }

    VertexBufferPtr DummyRenderer::CreateVertexBuffer(const float vertexData[], const size_t vertexDataSize, const uint32_t indexData[], const size_t indexDataSize)
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
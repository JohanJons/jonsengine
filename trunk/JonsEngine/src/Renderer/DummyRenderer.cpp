#include "include/Renderer/DummyRenderer.h"

#include "include/Core/EngineDefs.h"
#include "include/Core/Containers/Vector.h"

namespace JonsEngine
{
    DummyRenderer::DummyRenderer(const EngineSettings& engineSettings)
    {
    }
       
    DummyRenderer::~DummyRenderer()
    {
    }

        
    VertexBufferPtr DummyRenderer::CreateVertexBuffer(const vector<float>& vertexData, const vector<uint16_t>& indexData)
    {
        return VertexBufferPtr();
    }

    VertexBufferPtr DummyRenderer::CreateVertexBuffer(const float vertexData[], const size_t vertexDataSize, const uint16_t indexData[], const size_t indexDataSize)
    {
        return VertexBufferPtr();
    }

      
    void DummyRenderer::BeginRendering()
    {
    }

    void DummyRenderer::RenderMesh(MeshPtr mesh, const Mat4& modelMatrix, const Mat4& viewMatrix, const Mat4& projectionMatrix)
    {
    }

    void DummyRenderer::EndRendering()
    {
    }
}
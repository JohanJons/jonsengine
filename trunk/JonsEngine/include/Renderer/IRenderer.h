#pragma once

#include "include/Renderer/IVertexBuffer.h"
#include "include/Scene/Mesh.h"
#include "include/Core/Containers/Vector.h"
#include "include/Core/Types.h"

namespace JonsEngine
{
    class Mesh;

    /* IRenderer definition */
    class IRenderer
    {
    public:
        enum RenderBackendType
        {
            OPENGL = 0,
            NONE
        };

        virtual ~IRenderer() { }

        virtual VertexBufferPtr CreateVertexBuffer(const vector<float>& vertexData, const vector<uint16_t>& indexData) = 0;
        virtual VertexBufferPtr CreateVertexBuffer(const float vertexData[], const size_t vertexDataSize, const uint16_t indexData[], const size_t indexDataSize) = 0;
        
        virtual void BeginRendering() = 0;
        virtual void RenderMesh(MeshPtr mesh, const Mat4& modelMatrix, const Mat4& viewMatrix, const Mat4& projectionMatrix) = 0;
        virtual void EndRendering() = 0;
        
        virtual RenderBackendType GetRenderBackendType() const = 0;
    };
}
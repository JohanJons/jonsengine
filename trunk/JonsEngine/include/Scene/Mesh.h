#pragma once

#include "include/Renderer/IVertexBuffer.h"

namespace JonsEngine
{
    /* Mesh defintion */
    struct Mesh
    {
        VertexBufferPtr mVertexBuffer;

        Mesh(VertexBufferPtr vertexBuffer);
    };

    inline Mesh::Mesh(VertexBufferPtr vertexBuffer) : mVertexBuffer(vertexBuffer)
    {
    }
}
#pragma once

#include "include/Renderer/IVertexBuffer.h"
#include "include/Renderer/IMaterial.h"

namespace JonsEngine
{
    /* Mesh defintion */
    struct Mesh
    {
        VertexBufferPtr mVertexBuffer;
        MaterialPtr mMaterial;

        Mesh(VertexBufferPtr vertexBuffer, MaterialPtr material);
    };

    inline Mesh::Mesh(VertexBufferPtr vertexBuffer, MaterialPtr material) : mVertexBuffer(vertexBuffer), mMaterial(material)
    {
    }
}
#pragma once

#include "include/Renderer/IVertexBuffer.h"
#include "include/Scene/Material.h"

namespace JonsEngine
{
    /* Mesh defintion */
    struct Mesh
    {
        VertexBufferPtr mVertexBuffer;
        MaterialPtr mMaterial;


        Mesh(VertexBufferPtr vertexBuffer);
        Mesh(VertexBufferPtr vertexBuffer, MaterialPtr material);
    };


    /* Mesh inlines */
    inline Mesh::Mesh(VertexBufferPtr vertexBuffer) : mVertexBuffer(vertexBuffer)
    {
    }

    inline Mesh::Mesh(VertexBufferPtr vertexBuffer, MaterialPtr material) : mVertexBuffer(vertexBuffer), mMaterial(material)
    {
    }
}
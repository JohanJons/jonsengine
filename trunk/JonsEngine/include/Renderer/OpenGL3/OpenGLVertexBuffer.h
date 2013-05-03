#pragma once

#include "include/Renderer/IVertexBuffer.h"
#include "include/Core/EngineDefs.h"
#include "include/Core/Types.h"

#include "GL/glew.h"
#include <vector>

namespace JonsEngine
{
    class OpenGLVertexBuffer : public IVertexBuffer
    {
    public:
         OpenGLVertexBuffer(const std::vector<Vec3>& vertexData, const std::vector<Vec3>& normalData, const std::vector<Vec2>& texCoords, const std::vector<uint32_t>& indexData);
         ~OpenGLVertexBuffer();
 
        void Render();


    private:
        GLsizei mIndices;
        GLuint mVBO;
        GLuint mIndexBuffer;
        GLuint mVAO;
    };
}
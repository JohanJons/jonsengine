#pragma once

#include "include/Renderer/IVertexBuffer.h"
#include "include/Core/EngineDefs.h"

#include "GL/glew.h"
#include <vector>

namespace JonsEngine
{
    class OpenGLVertexBuffer : public IVertexBuffer
    {
    public:
         OpenGLVertexBuffer(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<uint32_t>& indexData);
         ~OpenGLVertexBuffer();
 
        void Render();


    private:
        GLsizei mIndices;
        GLuint mVBO;
        GLuint mIndexBuffer;
        GLuint mVAO;
    };
}
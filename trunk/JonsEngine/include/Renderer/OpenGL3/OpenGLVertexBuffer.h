#pragma once

#include "include/Renderer/IVertexBuffer.h"
#include "include/Core/EngineDefs.h"

#include <vector>
#include "GL/glew.h"

namespace JonsEngine
{
    class OpenGLVertexBuffer : public IVertexBuffer
    {
    public:
        OpenGLVertexBuffer(const std::vector<float>& vertexData, const std::vector<uint32_t>& indexData);
        OpenGLVertexBuffer(const float vertexData[], const size_t vertexDataSize, const uint32_t indexData[], const size_t indexDataSize);
        ~OpenGLVertexBuffer();

        void Render();


    private:
        void InitBuffers(const float vertexData[], const size_t vertexDataSize, const uint32_t indexData[], const size_t indexDataSize);

        GLsizei mIndices;
        GLuint mVBO;
        GLuint mIndexBuffer;
        GLuint mVAO;
    };
}
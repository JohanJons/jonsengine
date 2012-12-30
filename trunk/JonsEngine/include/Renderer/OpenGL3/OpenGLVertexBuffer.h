#pragma once

#include "include/Renderer/IVertexBuffer.h"
#include "include/Core/Containers/Vector.h"

#include "GL/glew.h"

namespace JonsEngine
{
    class OpenGLVertexBuffer : public IVertexBuffer
    {
    public:
        OpenGLVertexBuffer(const vector<float>& vertexData, const vector<uint16_t>& indexData);
        OpenGLVertexBuffer(const float vertexData[], const size_t vertexDataSize, const uint16_t indexData[], const size_t indexDataSize);
        ~OpenGLVertexBuffer();

        void Render();


    private:
        void InitBuffers(const float vertexData[], const size_t vertexDataSize, const uint16_t indexData[], const size_t indexDataSize);

        GLsizei mIndices;
        GLuint mVBO;
        GLuint mIndexBuffer;
        GLuint mVAO;
    };
}
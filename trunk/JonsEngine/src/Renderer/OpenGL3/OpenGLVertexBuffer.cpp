#include "include/Renderer/OpenGL3/OpenGLVertexBuffer.h"

namespace JonsEngine
{
    OpenGLVertexBuffer::OpenGLVertexBuffer(const std::vector<float>& vertexData, const std::vector<uint32_t>& indexData) : mIndices(vertexData.size())
    {
        InitBuffers(&vertexData[0], vertexData.size(), &indexData[0], indexData.size());
    }

    OpenGLVertexBuffer::OpenGLVertexBuffer(const float vertexData[], const size_t vertexDataSize, const uint32_t indexData[], const size_t indexDataSize) : mIndices(indexDataSize)
    {
        InitBuffers(vertexData, vertexDataSize, indexData, indexDataSize);
    }
        
    OpenGLVertexBuffer::~OpenGLVertexBuffer()
    {
        glDeleteBuffers(1, &mVBO);
        glDeleteBuffers(1, &mIndexBuffer);
        glDeleteVertexArrays(1, &mVAO);
    }

        
    void OpenGLVertexBuffer::Render()
    {
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, mIndices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }


    void OpenGLVertexBuffer::InitBuffers(const float vertexData[], const size_t vertexDataSize, const uint32_t indexData[], const size_t indexDataSize)
    {
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mIndexBuffer);
        glGenVertexArrays(1, &mVAO);

        // buffer vertex and index data
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, vertexDataSize * sizeof(float), vertexData, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize * sizeof(uint32_t), indexData, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        // unbind array buffer and VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}
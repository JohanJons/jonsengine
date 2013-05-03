#include "include/Renderer/OpenGL3/OpenGLVertexBuffer.h"

namespace JonsEngine
{
     OpenGLVertexBuffer::OpenGLVertexBuffer(const std::vector<Vec3>& vertexData, const std::vector<Vec3>& normalData, const std::vector<Vec2>& texCoords, const std::vector<uint32_t>& indexData) : mIndices(indexData.size())
     {
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mIndexBuffer);
        glGenVertexArrays(1, &mVAO);
 
        // buffer vertex, normals and index data
        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, (vertexData.size() + normalData.size()) * sizeof(Vec3), NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, NULL, vertexData.size() * sizeof(Vec3), &vertexData[0]);
        glBufferSubData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vec3), normalData.size() * sizeof(Vec3), &normalData[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(uint32_t), &indexData[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(vertexData.size() * sizeof(Vec3)));

       // unbind array buffer and VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
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
}
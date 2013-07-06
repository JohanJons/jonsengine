#include "include/Renderer/OpenGL3/OpenGLMesh.h"

namespace JonsEngine
{
     OpenGLMesh::OpenGLMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<uint32_t>& indexData) : mIndices(indexData.size())
     {
        glGenBuffers(1, &mVBO);
        glGenBuffers(1, &mIndexBuffer);
        glGenVertexArrays(1, &mVAO);
 
        // buffer vertex, normals and index data
        size_t vertexDataSize   = vertexData.size() * sizeof(float);
        size_t normalDataSize   = normalData.size() * sizeof(float);
        size_t texCoordDataSize = texCoords.size() * sizeof(float);
        size_t indexDataSize    = indexData.size() * sizeof(uint32_t);

        glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, vertexDataSize + normalDataSize + texCoordDataSize, NULL, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, NULL, vertexDataSize, &vertexData[0]);
        glBufferSubData(GL_ARRAY_BUFFER, vertexDataSize, normalDataSize, &normalData[0]);
        glBufferSubData(GL_ARRAY_BUFFER, vertexDataSize + normalDataSize, texCoordDataSize, &texCoords[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, &indexData[0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(vertexDataSize));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(vertexDataSize + normalDataSize));

       // unbind array buffer and VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
     }
        
    OpenGLMesh::~OpenGLMesh()
    {
        glDeleteBuffers(1, &mVBO);
        glDeleteBuffers(1, &mIndexBuffer);
        glDeleteVertexArrays(1, &mVAO);
    }

        
    void OpenGLMesh::Render()
    {
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, mIndices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
}
#include "include/Renderer/OpenGL3/OpenGLMesh.h"

#include "include/Renderer/OpenGL3/OpenGLUtils.hpp"
#include "include/Core/Logging/Logger.h"


namespace JonsEngine
{
     OpenGLMesh::OpenGLMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<uint32_t>& indexData) : mIndices(indexData.size()), mLogger(Logger::GetRendererLogger())
     {
        glGenBuffers(1, &mVBO);
        CHECK_GL_ERROR(mLogger);
        glGenBuffers(1, &mIndexBuffer);
        CHECK_GL_ERROR(mLogger);
       // glGenVertexArrays(1, &mVAO);
 
        // buffer vertex, normals and index data
         vertexDataSize   = vertexData.size() * sizeof(float);
         normalDataSize   = normalData.size() * sizeof(float);
        size_t texCoordDataSize = texCoords.size() * sizeof(float);
        size_t indexDataSize    = indexData.size() * sizeof(uint32_t);

        //glBindVertexArray(mVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        CHECK_GL_ERROR(mLogger);
        glBufferData(GL_ARRAY_BUFFER, vertexDataSize + normalDataSize + texCoordDataSize, NULL, GL_STATIC_DRAW);
        CHECK_GL_ERROR(mLogger);
        glBufferSubData(GL_ARRAY_BUFFER, NULL, vertexDataSize, &vertexData[0]);
        CHECK_GL_ERROR(mLogger);
        glBufferSubData(GL_ARRAY_BUFFER, vertexDataSize, normalDataSize, &normalData[0]);
        CHECK_GL_ERROR(mLogger);
        glBufferSubData(GL_ARRAY_BUFFER, vertexDataSize + normalDataSize, texCoordDataSize, &texCoords[0]);
        CHECK_GL_ERROR(mLogger);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
        CHECK_GL_ERROR(mLogger);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, &indexData[0], GL_STATIC_DRAW);

       // glEnableVertexAttribArray(0);
       // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
      //  glEnableVertexAttribArray(1);
       // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(vertexDataSize));
      //  glEnableVertexAttribArray(2);
     //   glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(vertexDataSize + normalDataSize));

       // unbind array buffer and VAO
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR(mLogger);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR(mLogger);
   //     glBindVertexArray(0);
        SetupVAO();
     }
        
    OpenGLMesh::~OpenGLMesh()
    {
        glDeleteBuffers(1, &mVBO);
        CHECK_GL_ERROR(mLogger);

        glDeleteBuffers(1, &mIndexBuffer);
        CHECK_GL_ERROR(mLogger);

        glDeleteVertexArrays(1, &mVAO);
        CHECK_GL_ERROR(mLogger);
    }

        
    void OpenGLMesh::Render()
    {
        glBindVertexArray(mVAO);
        CHECK_GL_ERROR(mLogger);

        glDrawElements(GL_TRIANGLES, mIndices, GL_UNSIGNED_INT, 0);
        CHECK_GL_ERROR(mLogger);

        glBindVertexArray(0);
        CHECK_GL_ERROR(mLogger);
    }


    void OpenGLMesh::SetupVAO()
    {
        glGenVertexArrays(1, &mVAO);
        CHECK_GL_ERROR(mLogger);

        glBindVertexArray(mVAO);
        CHECK_GL_ERROR(mLogger);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        CHECK_GL_ERROR(mLogger);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
        CHECK_GL_ERROR(mLogger);

        glEnableVertexAttribArray(0);
        CHECK_GL_ERROR(mLogger);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        CHECK_GL_ERROR(mLogger);
        glEnableVertexAttribArray(1);
        CHECK_GL_ERROR(mLogger);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(vertexDataSize));
        CHECK_GL_ERROR(mLogger);
        glEnableVertexAttribArray(2);
        CHECK_GL_ERROR(mLogger);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(vertexDataSize + normalDataSize));
        CHECK_GL_ERROR(mLogger);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR(mLogger);
        glBindVertexArray(0);
        CHECK_GL_ERROR(mLogger);
    }
}
#include "include/Renderer/OpenGL3/OpenGLMesh.h"

#include "include/Renderer/OpenGL3/OpenGLUtils.hpp"
#include "include/Core/Logging/Logger.h"


namespace JonsEngine
{
    OpenGLMesh::OpenGLMesh(const MeshID meshID, const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<uint32_t>& indexData) : mMeshID(meshID), mLogger(Logger::GetRendererLogger()), mIndices(indexData.size()), mVAO(0)
    {
        glGenBuffers(1, &mVBO);
        CHECK_GL_ERROR(mLogger);
        glGenBuffers(1, &mIndexBuffer);
        CHECK_GL_ERROR(mLogger);
 
        // buffer vertex, normals and index data
        vertexDataSize   = vertexData.size() * sizeof(float);
        normalDataSize   = normalData.size() * sizeof(float);
        size_t texCoordDataSize = texCoords.size() * sizeof(float);
        size_t indexDataSize    = indexData.size() * sizeof(uint32_t);

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

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR(mLogger);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR(mLogger);
    }
        
    OpenGLMesh::~OpenGLMesh()
    {
        glDeleteBuffers(1, &mVBO);
        CHECK_GL_ERROR(mLogger);

        glDeleteBuffers(1, &mIndexBuffer);
        CHECK_GL_ERROR(mLogger);

        if (mVAO)
        {
            glDeleteVertexArrays(1, &mVAO);
            CHECK_GL_ERROR(mLogger);
        }
    }
}
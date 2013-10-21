#include "include/Renderer/OpenGL3/OpenGLMesh.h"

#include "include/Renderer/OpenGL3/OpenGLUtils.hpp"
#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
    MeshID gNextMeshID = 1;

    OpenGLMesh::OpenGLMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<uint32_t>& indexData, Logger& logger) : mLogger(logger), mMeshID(gNextMeshID++), mIndices(indexData.size())
    {
        glGenBuffers(1, &mVBO);
        CHECK_GL_ERROR(mLogger);
        glGenBuffers(1, &mIndexBuffer);
        CHECK_GL_ERROR(mLogger);
 
        // buffer vertex, normals and index data
        mVertexDataSize   = vertexData.size() * sizeof(float);
        mNormalDataSize   = normalData.size() * sizeof(float);
        size_t texCoordDataSize = texCoords.size() * sizeof(float);
        size_t indexDataSize    = indexData.size() * sizeof(uint32_t);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        CHECK_GL_ERROR(mLogger);
        glBufferData(GL_ARRAY_BUFFER, mVertexDataSize + mNormalDataSize + texCoordDataSize, NULL, GL_STATIC_DRAW);
        CHECK_GL_ERROR(mLogger);
        glBufferSubData(GL_ARRAY_BUFFER, NULL, mVertexDataSize, &vertexData[0]);
        CHECK_GL_ERROR(mLogger);
        glBufferSubData(GL_ARRAY_BUFFER, mVertexDataSize, mNormalDataSize, &normalData[0]);
        CHECK_GL_ERROR(mLogger);
        glBufferSubData(GL_ARRAY_BUFFER, mVertexDataSize + mNormalDataSize, texCoordDataSize, &texCoords[0]);
        CHECK_GL_ERROR(mLogger);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
        CHECK_GL_ERROR(mLogger);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, &indexData[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR(mLogger);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        CHECK_GL_ERROR(mLogger);
    }

    OpenGLMesh::OpenGLMesh(const OpenGLMesh& mesh, Logger& logger) : mLogger(logger), mIndices(mesh.mIndices), mVBO(mesh.mVBO), mIndexBuffer(mesh.mIndexBuffer), mVertexDataSize(mesh.mVertexDataSize), mNormalDataSize(mesh.mNormalDataSize)
    {
    }
        
    OpenGLMesh::~OpenGLMesh()
    {
        glDeleteBuffers(1, &mVBO);
        CHECK_GL_ERROR(mLogger);

        glDeleteBuffers(1, &mIndexBuffer);
        CHECK_GL_ERROR(mLogger);
    }
}
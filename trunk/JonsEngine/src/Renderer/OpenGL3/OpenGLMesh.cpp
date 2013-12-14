#include "include/Renderer/OpenGL3/OpenGLMesh.h"

#include "include/Renderer/OpenGL3/OpenGLUtils.h"
#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
    MeshID gNextMeshID = 1;

    OpenGLMesh::OpenGLMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData, Logger& logger) : mLogger(logger), mMeshID(gNextMeshID++), mIndices(indexData.size())
    {
        GLCALL(glGenBuffers(1, &mVBO));
        GLCALL(glGenBuffers(1, &mIndexBuffer));
 
        // buffer vertex, normals and index data
        mVertexDataSize = vertexData.size() * sizeof(float);
        mNormalDataSize = normalData.size() * sizeof(float);
        mTexCoordsSize  = texCoords.size() * sizeof(float);
        mTangentsSize   = tangents.size() * sizeof(float);
        mBitangentsSize = bitangents.size() * sizeof(float);
        size_t indexDataSize = indexData.size() * sizeof(uint32_t);

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, mVBO));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, mVertexDataSize + mNormalDataSize + mTexCoordsSize + mTangentsSize + mBitangentsSize, 0, GL_STATIC_DRAW));
        GLCALL(glBufferSubData(GL_ARRAY_BUFFER, 0, mVertexDataSize, &vertexData[0]));
        GLCALL(glBufferSubData(GL_ARRAY_BUFFER, mVertexDataSize, mNormalDataSize, &normalData[0]));
        GLCALL(glBufferSubData(GL_ARRAY_BUFFER, mVertexDataSize + mNormalDataSize, mTexCoordsSize, &texCoords[0]));
        GLCALL(glBufferSubData(GL_ARRAY_BUFFER, mVertexDataSize + mNormalDataSize + mTexCoordsSize, mTangentsSize, &tangents[0]));
        GLCALL(glBufferSubData(GL_ARRAY_BUFFER, mVertexDataSize + mNormalDataSize + mTexCoordsSize + mTangentsSize, mBitangentsSize, &bitangents[0]));
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer));
        GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, &indexData[0], GL_STATIC_DRAW));

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

    OpenGLMesh::OpenGLMesh(const OpenGLMesh& mesh, Logger& logger) : mLogger(logger), mIndices(mesh.mIndices), mVBO(mesh.mVBO), mIndexBuffer(mesh.mIndexBuffer), mVertexDataSize(mesh.mVertexDataSize), mNormalDataSize(mesh.mNormalDataSize)
    {
    }
        
    OpenGLMesh::~OpenGLMesh()
    {
        GLCALL(glDeleteBuffers(1, &mVBO));
        GLCALL(glDeleteBuffers(1, &mIndexBuffer));
    }
}
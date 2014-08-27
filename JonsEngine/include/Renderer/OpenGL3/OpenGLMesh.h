#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Types.h"

#include "GL/glew.h"
#include <vector>
#include <memory>

namespace JonsEngine
{
    class Logger;
    struct OpenGLMesh;

    /*
     * OpenGLMeshPtr definition
     */
    typedef std::shared_ptr<OpenGLMesh> OpenGLMeshPtr;

    /*
     * OpenGLMesh definition
     */
    struct OpenGLMesh
    {
        OpenGLMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData, Logger& logger);
        OpenGLMesh(const OpenGLMesh& mesh, Logger& logger);
        ~OpenGLMesh();

        Logger& mLogger;
        MeshID mMeshID;

        GLsizei mIndices;
        GLuint mVBO;
        GLuint mIndexBuffer;

        size_t mVertexDataSize;
        size_t mNormalDataSize;
        size_t mTexCoordsSize;
        size_t mTangentsSize;
        size_t mBitangentsSize;
    };
}
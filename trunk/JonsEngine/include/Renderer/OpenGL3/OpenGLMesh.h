#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Types.h"

#include "GL/glew.h"
#include <vector>

namespace JonsEngine
{
    class Logger;

    struct OpenGLMesh
    {
        OpenGLMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<uint32_t>& indexData, Logger& logger);
        OpenGLMesh(const OpenGLMesh& mesh, Logger& logger);
        ~OpenGLMesh();

        Logger& mLogger;
        MeshID mMeshID;

        GLsizei mIndices;
        GLuint mVBO;
        GLuint mIndexBuffer;

        size_t mVertexDataSize;
        size_t mNormalDataSize;
    };
}
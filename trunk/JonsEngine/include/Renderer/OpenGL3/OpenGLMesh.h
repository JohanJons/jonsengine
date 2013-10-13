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
        OpenGLMesh(const MeshID meshID, const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<uint32_t>& indexData);
        ~OpenGLMesh();

        MeshID mMeshID;
        Logger& mLogger;

        GLsizei mIndices;
        GLuint mVBO;
        GLuint mIndexBuffer;
        GLuint mVAO;

        size_t vertexDataSize;
        size_t normalDataSize;
    };
}
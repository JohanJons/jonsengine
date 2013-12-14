#include "include/Renderer/OpenGL3/OpenGLUtils.h"

namespace JonsEngine
{
    OpenGLMesh CreateSphereMesh(const float radius, const uint32_t rings, const uint32_t sectors, Logger& logger)
    {
        std::vector<float> vertexData, normalData, texcoordData, tangents, bitangents;
        std::vector<uint32_t> indiceData;
        if (!CreateSphereData(radius, rings, sectors, vertexData, normalData, texcoordData, indiceData))
        {
            JONS_LOG_ERROR(logger, "Failed to create sphere for shading pass");
            throw std::runtime_error("Failed to create sphere for shading pass");
        }

        return OpenGLMesh(vertexData, normalData, texcoordData, tangents, bitangents, indiceData, logger);
    }

    OpenGLMesh CreateRectangleMesh(const float sizeX, const float sizeY, const float sizeZ, Logger& logger)
    {
        std::vector<float> vertexData, normalData, texcoordData, tangents, bitangents;
        std::vector<uint32_t> indiceData;
        if (!CreateRectangleData(sizeX, sizeY, sizeZ, vertexData, normalData, texcoordData, indiceData))
        {
            JONS_LOG_ERROR(logger, "Failed to create rectangle");
            throw std::runtime_error("Failed to create rectangle");
        }

        return OpenGLMesh(vertexData, normalData, texcoordData, tangents, bitangents, indiceData, logger);
    }

    GLuint SetupVAO(const OpenGLMesh& mesh)
    {
        GLuint vao;

        GLCALL(glGenVertexArrays(1, &vao));

        GLCALL(glBindVertexArray(vao));
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, mesh.mVBO));
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.mIndexBuffer));

        GLCALL(glEnableVertexAttribArray(0));
        GLCALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0));
        GLCALL(glEnableVertexAttribArray(1));
        GLCALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh.mVertexDataSize)));
        GLCALL(glEnableVertexAttribArray(2));
        GLCALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh.mVertexDataSize + mesh.mNormalDataSize)));
        GLCALL(glEnableVertexAttribArray(3));
        GLCALL(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh.mVertexDataSize + mesh.mNormalDataSize + mesh.mTexCoordsSize)));
        GLCALL(glEnableVertexAttribArray(4));
        GLCALL(glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh.mVertexDataSize + mesh.mNormalDataSize + mesh.mTexCoordsSize + mesh.mTangentsSize)));

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCALL(glBindVertexArray(0));

        return vao;
    }
}
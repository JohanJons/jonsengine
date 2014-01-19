#include "include/Renderer/OpenGL3/OpenGLUtils.h"

namespace JonsEngine
{
    enum VertexAttribute
    {
        VERTEX_ATTRIBUTE_POSITION = 0,
        VERTEX_ATTRIBUTE_NORMAL,
        VERTEX_ATTRIBUTE_TEXCOORD,
        VERTEX_ATTRIBUTE_TANGENT,
        VERTEX_ATTRIBUTE_BITANGENT
    };


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

        GLCALL(glEnableVertexAttribArray(VertexAttribute::VERTEX_ATTRIBUTE_POSITION));
        GLCALL(glEnableVertexAttribArray(VertexAttribute::VERTEX_ATTRIBUTE_NORMAL));
        GLCALL(glEnableVertexAttribArray(VertexAttribute::VERTEX_ATTRIBUTE_TEXCOORD));
        GLCALL(glEnableVertexAttribArray(VertexAttribute::VERTEX_ATTRIBUTE_TANGENT));
        GLCALL(glEnableVertexAttribArray(VertexAttribute::VERTEX_ATTRIBUTE_BITANGENT));

        GLCALL(glVertexAttribPointer(VertexAttribute::VERTEX_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0));
        GLCALL(glVertexAttribPointer(VertexAttribute::VERTEX_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh.mVertexDataSize)));
        GLCALL(glVertexAttribPointer(VertexAttribute::VERTEX_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh.mVertexDataSize + mesh.mNormalDataSize)));
        GLCALL(glVertexAttribPointer(VertexAttribute::VERTEX_ATTRIBUTE_TANGENT, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh.mVertexDataSize + mesh.mNormalDataSize + mesh.mTexCoordsSize)));
        GLCALL(glVertexAttribPointer(VertexAttribute::VERTEX_ATTRIBUTE_BITANGENT, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(mesh.mVertexDataSize + mesh.mNormalDataSize + mesh.mTexCoordsSize + mesh.mTangentsSize)));

        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCALL(glBindVertexArray(0));

        return vao;
    }
}
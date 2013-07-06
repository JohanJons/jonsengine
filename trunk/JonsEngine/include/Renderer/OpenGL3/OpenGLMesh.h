#pragma once

#include "include/Renderer/IMesh.h"
#include "include/Core/EngineDefs.h"
#include "include/Core/Types.h"

#include "GL/glew.h"
#include <vector>

namespace JonsEngine
{
    class OpenGLMesh : public IMesh
    {
    public:
         OpenGLMesh(const std::vector<Vec3>& vertexData, const std::vector<Vec3>& normalData, const std::vector<Vec2>& texCoords, const std::vector<uint32_t>& indexData);
         ~OpenGLMesh();
 
        void Render();


    private:
        GLsizei mIndices;
        GLuint mVBO;
        GLuint mIndexBuffer;
        GLuint mVAO;
    };
}
#pragma once

#include "include/Core/EngineDefs.h"

#include "GL/glew.h"
#include <vector>

namespace JonsEngine
{
    class OpenGLMaterial
    {
    public:
        OpenGLMaterial(/*const std::vector<uint8_t> buffer*/);
        ~OpenGLMaterial();


    private:
        GLuint mTexture;
    };
}
#pragma once

#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Core/Types.h"

#include "GL/glew.h"
#include <string>
#include <vector>

namespace JonsEngine
{
    class Logger;

    /*
     * C++ wrapper around GLSL shaderprograms
     */

    /* ShaderProgram definition */
    struct ShaderProgram
    {
        ShaderProgram(const std::string& name, ShaderPtr vertexShader, ShaderPtr fragmentShader, Logger& logger);
        ~ShaderProgram();

        Logger& mLogger;
        const std::string mName;
        GLuint mProgramHandle;
        ShaderPtr mVertexShader;
        ShaderPtr mFragmentShader;
    };
}
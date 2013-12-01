#pragma once

#include "GL/glew.h"
#include <string>
#include <memory>
#include <functional>

namespace JonsEngine
{
    class Logger;

    /*
     * C++ wrapper around GLSL shaders
     */

    struct Shader;

    /* Shader definition */
    typedef std::unique_ptr<Shader> ShaderPtr;

    /* Shader definition */
    struct Shader
    {
        friend struct ShaderProgram;

        enum ShaderType
        {
            VERTEX_SHADER = GL_VERTEX_SHADER,
            FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
            UNKNOWN
        };
        
        Shader(const std::string& name, const std::string& shaderSource, ShaderType type);
        ~Shader();

        Logger& mLogger;
        const std::string mName;
        const ShaderType mShaderType;
        GLuint mShaderHandle;
    };
}
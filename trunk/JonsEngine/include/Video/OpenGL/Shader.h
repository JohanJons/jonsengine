#pragma once

#include "GL/glew.h"

#include <string>

namespace JonsEngine
{
    class Logger;

    class Shader
    {
    public:
        friend class ShaderProgram;

        enum ShaderType
        {
            VERTEX_SHADER = GL_VERTEX_SHADER,
            FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
            UNKNOWN
        };

        Shader(const std::string& name, ShaderType type);
        ~Shader();

        bool Compile(const std::string& shader);

        void SetName(const std::string& name);
        const std::string& GetName() const;
        bool IsCompiled() const;

        
    protected:
        Logger& mLogger;
        std::string mName;
        const ShaderType mShaderType;
        GLuint mShaderHandle;
        bool mIsCompiled;

    };

}
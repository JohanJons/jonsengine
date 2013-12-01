#include "include/Renderer/OpenGL3/ShaderProgram.h"

#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/OpenGLUtils.hpp"
#include "include/Core/Logging/Logger.h"

#include <exception>

namespace JonsEngine
{

    ShaderProgram::ShaderProgram(const std::string& name, ShaderPtr vertexShader, ShaderPtr fragmentShader, Logger& logger) : mLogger(logger), mName(name), mProgramHandle(glCreateProgram()), mVertexShader(std::move(vertexShader)), mFragmentShader(std::move(fragmentShader))
    {
        if (mVertexShader)
            GLCALL(glAttachShader(mProgramHandle, mVertexShader->mShaderHandle));
        if (mFragmentShader)
            GLCALL(glAttachShader(mProgramHandle, mFragmentShader->mShaderHandle));

        GLCALL(glLinkProgram(mProgramHandle));
        GLCALL(glValidateProgram(mProgramHandle));

        GLint status;
        GLCALL(glGetProgramiv(mProgramHandle, GL_VALIDATE_STATUS, &status));
        if (status != GL_TRUE)
        {
            JONS_LOG_ERROR(logger, "ShaderProgram failed to compile");
            throw std::runtime_error("ShaderProgram failed to compile");
        }
    }
        
    ShaderProgram::~ShaderProgram()
    {
        GLCALL(glDeleteProgram(mProgramHandle));
    }
}
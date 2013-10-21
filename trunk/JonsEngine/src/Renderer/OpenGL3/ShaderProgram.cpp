#include "include/Renderer/OpenGL3/ShaderProgram.h"

#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/OpenGLUtils.hpp"
#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{

    ShaderProgram::ShaderProgram(const std::string& name, const std::string& vertexShaderSource, const std::string& fragmentShaderSource) : mLogger(Logger::GetRendererLogger()), mName(name), mProgramHandle(glCreateProgram()), mVertexShader("DefaultVertexShader", vertexShaderSource, Shader::VERTEX_SHADER), mFragmentShader("DefaultFragmentShader", fragmentShaderSource, Shader::FRAGMENT_SHADER)
    {
        glAttachShader(mProgramHandle, mVertexShader.mShaderHandle);
        CHECK_GL_ERROR(mLogger);

        glAttachShader(mProgramHandle, mFragmentShader.mShaderHandle);
        CHECK_GL_ERROR(mLogger);

        glLinkProgram(mProgramHandle);
        CHECK_GL_ERROR(mLogger);

        glUseProgram(mProgramHandle);
        CHECK_GL_ERROR(mLogger);
    }
        
    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(mProgramHandle);
    }

    void ShaderProgram::BindAttribLocation(GLuint index, const std::string& name)
    {
        glBindAttribLocation(mProgramHandle, index, name.c_str());
        CHECK_GL_ERROR(mLogger);
    }


    const std::string& ShaderProgram::GetName() const
    {
        return mName;
    }

    GLuint ShaderProgram::GetHandle() const
    {
        return mProgramHandle;
    }
}
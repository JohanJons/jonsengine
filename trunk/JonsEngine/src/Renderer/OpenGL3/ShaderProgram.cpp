#include "include/Renderer/OpenGL3/ShaderProgram.h"

#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/UniformBuffer.hpp"
#include "include/Renderer/OpenGL3/OpenGLUtils.hpp"
#include "include/Core/Logging/Logger.h"

#include "boost/bind.hpp"
#include "boost/foreach.hpp"

#include <algorithm>

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

    void ShaderProgram::BindTextureUnit(ITexture::TextureType textureType, GLuint textureUnit)
    {
        switch (textureType)
        {
            case ITexture::DIFFUSE:
            {
                GLuint textureUnifLocation = glGetUniformLocation(mProgramHandle, "diffuseTexture");
                glUniform1i(textureUnifLocation, textureUnit);
                CHECK_GL_ERROR(mLogger);
            }

            default:
                break;
        }
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
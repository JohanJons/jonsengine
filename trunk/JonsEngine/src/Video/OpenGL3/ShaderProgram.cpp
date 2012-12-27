#include "include/Video/OpenGL3/ShaderProgram.h"

#include "include/Video/OpenGL3/Shader.h"
#include "include/Video/OpenGL3/UniformBuffer.h"
#include "include/Core/Logging/Logger.h"

#include "boost/bind.hpp"
#include "boost/foreach.hpp"

#include <algorithm>

namespace JonsEngine
{

    ShaderProgram::ShaderProgram(const std::string& name) : mLogger(Logger::GetVideoLogger()), mName(name), mProgramHandle(glCreateProgram())
    {
        glGenBuffers(1, &mUniformBuffer);
    }
        
    ShaderProgram::~ShaderProgram()
    {
        ClearShaders();

        glDeleteProgram(mProgramHandle);
        glDeleteBuffers(1, &mUniformBuffer);
        mProgramHandle = 0;
    }

        
    void ShaderProgram::AddShader(Shader* shader)
    {   
        if (!shader->IsCompiled())
        {
            JONS_LOG_ERROR(mLogger, "ShaderProgram::AddShader(): Shader not compiled");
            return;
        }

        mAddedShaders.push_back(shader);
        glAttachShader(mProgramHandle, shader->mShaderHandle);
    }

    void ShaderProgram::RemoveShader(Shader* shader)
    {
        mAddedShaders.erase(std::find(mAddedShaders.begin(), mAddedShaders.end(), shader));
        glDetachShader(mProgramHandle, shader->mShaderHandle);
    }

    void ShaderProgram::ClearShaders()
    {
        BOOST_FOREACH(Shader* shader, mAddedShaders)
            glDetachShader(mProgramHandle, shader->mShaderHandle);

        mAddedShaders.clear();
    }
        
    bool ShaderProgram::LinkProgram()
    {
        glLinkProgram(mProgramHandle);

        if (!IsLinked())
        {
            JONS_LOG_ERROR(mLogger, "ShaderProgram::LinkProgram(): Failed to link program");
            return false;
        }
        else
            return true;
    }
        
    void ShaderProgram::UseProgram(bool use)
    {
        if (!IsLinked())
        {
            JONS_LOG_ERROR(mLogger, "ShaderProgram::UseProgram(): Program not linked");
            return;
        }

        if (use)
            glUseProgram(mProgramHandle);
        else
            glUseProgram(0);
    }

    void ShaderProgram::BindAttribLocation(GLuint index, const std::string& name)
    {
        glBindAttribLocation(mProgramHandle, index, name.c_str());
    }

    void ShaderProgram::UseUniform(UniformBuffer& buffer, bool use)
    {
        GLuint blockIndex = glGetUniformBlockIndex(mProgramHandle, buffer.mName.c_str());

        glUniformBlockBinding(mProgramHandle, blockIndex, use ? buffer.mBindingIndex : 0);
        glBindBufferRange(GL_UNIFORM_BUFFER, buffer.mBindingIndex, buffer.mBuffer, 0, buffer.mBufferSize);
    }

    const std::string& ShaderProgram::GetName() const
    {
        return mName;
    }

    bool ShaderProgram::IsLinked() const
    {
        GLint isLinked;

        glGetProgramiv(mProgramHandle, GL_LINK_STATUS, &isLinked);

        return isLinked == GL_TRUE ? true : false;
    }
}
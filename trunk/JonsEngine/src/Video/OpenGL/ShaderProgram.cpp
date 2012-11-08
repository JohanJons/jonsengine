#include "include/Video/OpenGL/ShaderProgram.h"

#include "include/Video/OpenGL/Shader.h"
#include "include/Core/Logging/Logger.h"

#include "boost/bind.hpp"
#include "boost/foreach.hpp"

#include <algorithm>

namespace JonsEngine
{

    ShaderProgram::ShaderProgram(const std::string& name) : mLogger(Globals::GetVideoLogger()), mName(name), mIsLinked(false), mProgramHandle(0)
    {
    }
        
    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(mProgramHandle);
        mProgramHandle = 0;
        mIsLinked = false;

        ClearShaders();
    }

        
    void ShaderProgram::AddShader(Shader* shader)
    {   
        if (shader->IsCompiled())
            mAddedShaders.push_back(shader);
        else
            JONS_LOG_ERROR(mLogger, "ShaderProgram::AddShader(): Shader not compiled");
    }

    void ShaderProgram::RemoveShader(Shader* shader)
    {
        mAddedShaders.erase(std::find(mAddedShaders.begin(), mAddedShaders.end(), shader));
    }

    void ShaderProgram::ClearShaders()
    {
        mAddedShaders.clear();
    }

    void ShaderProgram::BindAttribLocation(GLuint index, const std::string& name)
    {
        if (mProgramHandle == 0)
            mProgramHandle = glCreateProgram();

        glBindAttribLocation(mProgramHandle, index, name.c_str());
    }
        
    bool ShaderProgram::LinkProgram()
    {
        if (mIsLinked)
        {
            JONS_LOG_ERROR(mLogger, "ShaderProgram::LinkProgram(): Program already linked");
            return false;
        }

        if (mProgramHandle == 0)
            mProgramHandle = glCreateProgram();

        BOOST_FOREACH(Shader* shader, mAddedShaders)
            glAttachShader(mProgramHandle, shader->mShaderHandle);

        glLinkProgram(mProgramHandle);

        GLint status;
        glGetProgramiv (mProgramHandle, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
            JONS_LOG_ERROR(mLogger, "ShaderProgram::LinkProgram(): Failed to link program");
            return false;
        }

        BOOST_FOREACH(Shader* shader, mAddedShaders)
            glDetachShader(mProgramHandle, shader->mShaderHandle);

        mIsLinked = true;
        return true;
    }

    void ShaderProgram::UnlinkProgram()
    {
        if (!mIsLinked)
            JONS_LOG_ERROR(mLogger, "ShaderProgram::UnlinkProgram(): Program not linked");
    }
        
    void ShaderProgram::UseProgram(bool use)
    {
        if (use)
            glUseProgram(mProgramHandle);
        else
            glUseProgram(0);
    }

    const std::string& ShaderProgram::GetName() const
    {
        return mName;
    }

    bool ShaderProgram::IsLinked() const
    {
        return mIsLinked;
    }
}
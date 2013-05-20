#include "include/Renderer/OpenGL3/ShaderProgram.h"

#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/UniformBuffer.hpp"
#include "include/Core/Logging/Logger.h"

#include "boost/bind.hpp"
#include "boost/foreach.hpp"

#include <algorithm>

namespace JonsEngine
{

    ShaderProgram::ShaderProgram(const std::string& name) : mLogger(Logger::GetRendererLogger()), mName(name), mProgramHandle(glCreateProgram())
    {
    }
        
    ShaderProgram::~ShaderProgram()
    {
        ClearShaders();

        glDeleteProgram(mProgramHandle);
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

    Shader* ShaderProgram::GetShader(const std::string& name)
    {
        Shader* ret = NULL;
        std::vector<Shader*>::iterator iter = std::find_if(mAddedShaders.begin(), mAddedShaders.end(), boost::bind(&Shader::mName, _1) == name);

        if (iter != mAddedShaders.end())
            ret = *iter;

        return ret;
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

    void ShaderProgram::BindTextureUnit(ITexture::TextureType textureType, GLuint textureUnit)
    {
        switch (textureType)
        {
            case ITexture::DIFFUSE:
            {
                GLuint textureUnifLocation = glGetUniformLocation(mProgramHandle, "diffuseTexture");
                glUniform1i(textureUnifLocation, textureUnit);
            }

            default:
                break;
        }
    }


    const std::string& ShaderProgram::GetName() const       { return mName;          }
    GLuint ShaderProgram::GetHandle() const                 { return mProgramHandle; }

    bool ShaderProgram::IsLinked() const
    {
        GLint isLinked;

        glGetProgramiv(mProgramHandle, GL_LINK_STATUS, &isLinked);

        return isLinked == GL_TRUE ? true : false;
    }
}
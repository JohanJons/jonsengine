#pragma once

#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/UniformBuffer.hpp"
#include "include/Core/Types.h"
#include "include/Core/Logging/Logger.h"

#include "GL/glew.h"
#include <string>
#include <vector>

namespace JonsEngine
{
    /*
     * C++ wrapper around GLSL shaderprograms
     */

    /* ShaderProgram definition */
    template <typename T>
    class ShaderProgram
    {
    public:
        ShaderProgram(const std::string& programName, ShaderPtr vertexShader, ShaderPtr fragmentShader, Logger& logger, const std::string& unifBufferName)
            : 
            mLogger(logger), mName(programName), mVertexShader(std::move(vertexShader)), mFragmentShader(std::move(fragmentShader)), mProgramHandle(createProgram()), mUniformBuffer(unifBufferName, logger, mProgramHandle)
        {
        }

        ~ShaderProgram()
        {
            GLCALL(glDeleteProgram(mProgramHandle));
        }


        void UseProgram()
        {
            GLCALL(glUseProgram(mProgramHandle));
        }

        void SetUniformData(const T& uniformData)
        {
            mUniformBuffer.SetData(uniformData);
        }


    private:
        GLuint createProgram()
        {
            GLuint programID = glCreateProgram();

            if (mVertexShader)
                GLCALL(glAttachShader(programID, mVertexShader->mShaderHandle));
            if (mFragmentShader)
                GLCALL(glAttachShader(programID, mFragmentShader->mShaderHandle));

            GLCALL(glLinkProgram(programID));
            GLCALL(glValidateProgram(programID));

            GLint status;
            GLCALL(glGetProgramiv(programID, GL_VALIDATE_STATUS, &status));
            if (status != GL_TRUE)
            {
                JONS_LOG_ERROR(mLogger, "ShaderProgram failed to compile");
                throw std::runtime_error("ShaderProgram failed to compile");
            }

            return programID;
        }


        Logger& mLogger;
        const std::string mName;
        ShaderPtr mVertexShader;
        ShaderPtr mFragmentShader;
        GLuint mProgramHandle;
        UniformBuffer<T> mUniformBuffer;
    };
}
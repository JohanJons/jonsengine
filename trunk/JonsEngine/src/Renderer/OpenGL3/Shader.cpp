#include "include/Renderer/OpenGL3/Shader.h"

#include "include/Renderer/OpenGL3/OpenGLUtils.hpp"
#include "include/Core/Logging/Logger.h"

#include <vector>
#include <exception>

namespace JonsEngine
{

    Shader::Shader(const std::string& name, const std::string& shaderSource, ShaderType type) : mLogger(Logger::GetRendererLogger()), mName(name), mShaderHandle(0), mShaderType(type)
    {
        mShaderHandle = glCreateShader(mShaderType);
        const char* shaderCSTR = shaderSource.c_str();

        glShaderSource(mShaderHandle, 1, &shaderCSTR, NULL);
        CHECK_GL_ERROR(mLogger);

        glCompileShader(mShaderHandle);
        CHECK_GL_ERROR(mLogger);

        GLint status;
        glGetShaderiv(mShaderHandle, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint errorLogSize = 0;
            glGetShaderiv(mShaderHandle, GL_INFO_LOG_LENGTH, &errorLogSize);
            std::vector<GLchar> errorLog(errorLogSize);
 
	        glGetShaderInfoLog(mShaderHandle, errorLogSize, &errorLogSize, &errorLog[0]);

            std::string msg = "Shader::Compile(): Failed to compile shader: "; 
            msg.append(&errorLog[0]);

            JONS_LOG_ERROR(mLogger, msg);
            throw std::runtime_error(msg);
        }
    }
        
    Shader::~Shader()
    {
        glDeleteShader(mShaderHandle);
        CHECK_GL_ERROR(mLogger);
    }
        

    const std::string& Shader::GetName() const
    {
        return mName;
    }

    Shader::ShaderType Shader::GetShaderType() const
    {
        return mShaderType;
    }
}
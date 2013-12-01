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

        GLCALL(glShaderSource(mShaderHandle, 1, &shaderCSTR, 0));

        GLCALL(glCompileShader(mShaderHandle));

        GLint status;
        GLCALL(glGetShaderiv(mShaderHandle, GL_COMPILE_STATUS, &status));
        if (status == GL_FALSE)
        {
            GLint errorLogSize = 0;
            GLCALL(glGetShaderiv(mShaderHandle, GL_INFO_LOG_LENGTH, &errorLogSize));
            std::vector<GLchar> errorLog(errorLogSize);
 
	        GLCALL(glGetShaderInfoLog(mShaderHandle, errorLogSize, &errorLogSize, &errorLog[0]));

            std::string msg = "Shader::Compile(): Failed to compile shader ";
            msg.append(mName);
            msg.append(": ");
            msg.append(&errorLog[0]);

            JONS_LOG_ERROR(mLogger, msg);
            throw std::runtime_error(msg);
        }
    }
        
    Shader::~Shader()
    {
        GLCALL(glDeleteShader(mShaderHandle));
    }
}
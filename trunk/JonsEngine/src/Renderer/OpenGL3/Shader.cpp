#include "include/Renderer/OpenGL3/Shader.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/EngineDefs.h"

#include <array>

namespace JonsEngine
{

    Shader::Shader(const std::string& name, ShaderType type) : mLogger(Logger::GetRendererLogger()), mName(name), mShaderHandle(0), mShaderType(type), mIsCompiled(false)
    {
    }
        
    Shader::~Shader()
    {
        glDeleteShader(mShaderHandle);
        mShaderHandle = 0;
        mIsCompiled = false;
    }
        
    bool Shader::Compile(const std::string& shaderSource)
    {
        if (mIsCompiled)
        {
            JONS_LOG_ERROR(mLogger, "Shader::Compile(): Shader already compiled with a shaderprogram");
            return false;
        }

        mShaderHandle = glCreateShader(mShaderType);
        const char* shaderCSTR = shaderSource.c_str();
        glShaderSource(mShaderHandle, 1, &shaderCSTR, NULL);

        glCompileShader(mShaderHandle);

        GLint status;
        glGetShaderiv(mShaderHandle, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint errorLogSize = 250;
            std::array<GLchar, 250> errorLog;

            glGetShaderiv(mShaderHandle, GL_INFO_LOG_LENGTH, &errorLogSize);
 
	        glGetShaderInfoLog(mShaderHandle, errorLogSize, &errorLogSize, &errorLog[0]);

            std::string msg = "Shader::Compile(): Failed to compile shader: "; 
            msg.append(&errorLog[0]);

            JONS_LOG_ERROR(mLogger, msg);
            return false;
        }
        
        mIsCompiled = true;
        return true;
    }

    const std::string& Shader::GetName() const
    {
        return mName;
    }

    bool Shader::IsCompiled() const
    {
        return mIsCompiled;
    }
}
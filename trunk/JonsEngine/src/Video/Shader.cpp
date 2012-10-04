#include "include/Video/Shader.h"

#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{

	Shader::Shader(const std::string& name, ShaderType type) : mLogger(Globals::GetVideoLogger()), mName(name), mShaderHandle(0), mShaderType(type), mIsCompiled(false)
	{
	}
		
	Shader::~Shader()
	{
		Destroy();
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
			JONS_LOG_ERROR(mLogger, "Shader::Compile(): Failed to compile shader");

			GLint infoLogLength;
        glGetShaderiv(mShaderHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetShaderInfoLog(mShaderHandle, infoLogLength, NULL, strInfoLog);
			Destroy();
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

	void Shader::Destroy()
	{
		glDeleteShader(mShaderHandle);
		mShaderHandle = 0;
	}

}
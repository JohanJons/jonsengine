#include "include/Video/ShaderProgram.h"

#include "include/Video/Shader.h"
#include "include/Core/Logging/Logger.h"

#include "boost/bind.hpp"

#include <algorithm>

namespace JonsEngine
{

	ShaderProgram::ShaderProgram(const std::string& name) : mLogger(Globals::GetVideoLogger()), mName(name), mIsLinked(false), mProgramHandle(0)
	{
	}
		
	ShaderProgram::~ShaderProgram()
	{
		Destroy();
	}

		
	void ShaderProgram::AddShader(Shader* shader)
	{	
		mAddedShaders.push_back(shader);
	}

	void ShaderProgram::RemoveShader(Shader* shader)
	{
		mAddedShaders.erase(std::find(mAddedShaders.begin(), mAddedShaders.end(), shader));
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

		std::for_each(mAddedShaders.begin(), mAddedShaders.end(), boost::bind(&ShaderProgram::AttachShader, this, _1));

		glLinkProgram(mProgramHandle);

		GLint status;
		glGetProgramiv (mProgramHandle, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			JONS_LOG_ERROR(mLogger, "ShaderProgram::LinkProgram(): Failed to link program");
			Destroy();
			return false;
		}

		std::for_each(mAddedShaders.begin(), mAddedShaders.end(), boost::bind(&ShaderProgram::DetachShader, this, _1));

		mIsLinked = true;
		return true;
	}
		
	void ShaderProgram::UseProgram()
	{
		glUseProgram(mProgramHandle);
	}

	bool ShaderProgram::IsLinked() const
	{
		return mIsLinked;
	}

	void ShaderProgram::Destroy()
	{
		glDeleteProgram(mProgramHandle);
		mProgramHandle = 0;
	}

	void ShaderProgram::AttachShader(Shader* shader)
	{
		glAttachShader(mProgramHandle, shader->mShaderHandle);
	}

	void ShaderProgram::DetachShader(Shader* shader)
	{
		glDetachShader(mProgramHandle, shader->mShaderHandle);
	}
}
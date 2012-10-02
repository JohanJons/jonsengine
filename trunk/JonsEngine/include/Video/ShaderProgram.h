#pragma once

#include "include/Core/Containers/Vector.h"

#include "GL/glew.h"

#include <string>

namespace JonsEngine
{
	class Logger;
	class Shader;

	class ShaderProgram
	{
	public:
		ShaderProgram(const std::string& name);
		~ShaderProgram();

		void AddShader(Shader* shader);
		void RemoveShader(Shader* shader);

		void BindAttribLocation(GLuint index, const std::string& name);
		bool LinkProgram();
		void UseProgram();
		bool IsLinked() const;

	private:
		void Destroy();
		void AttachShader(Shader* shader);
		void DetachShader(Shader* shader);

		Logger& mLogger;
		const std::string mName;
		bool mIsLinked;
		GLuint mProgramHandle;
		Vector<Shader*> mAddedShaders;
	};


}
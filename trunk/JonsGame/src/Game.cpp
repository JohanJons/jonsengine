#include "include/Game.h"
#include "include/Shaders.h"

#include "include/Video/OpenGL/Shader.h"
#include "include/Video/OpenGL/ShaderProgram.h"
#include "include/Core/Utils/Types.h"

#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace JonsEngine;

namespace JonsGame
{

	Game::Game()
	{
	}
		
	Game::~Game()
	{
	}

		
	void Game::Run()
	{
		mEngine.Init();

		Triangle tri;
		tri.vecA.x = -0.5; tri.vecA.y = -0.5; tri.vecA.z = 0.0;
		tri.vecB.x = 0.0; tri.vecB.y = 0.5; tri.vecB.z = 0.0;
		tri.vecC.x = 0.5; tri.vecC.y = -0.5; tri.vecC.z = 0.0;

		Shader vertexShader("VertexShader", Shader::VERTEX_SHADER);
		Shader fragmentShader("FragmentShader", Shader::FRAGMENT_SHADER);

		vertexShader.Compile(gVertexShader);
		fragmentShader.Compile(gFragmentShader);

		if (vertexShader.IsCompiled() && fragmentShader.IsCompiled())
		{
			ShaderProgram shaderProgram("MyShaderProgram");
			shaderProgram.AddShader(&vertexShader);
			shaderProgram.AddShader(&fragmentShader);

			shaderProgram.BindAttribLocation(0, "position");

			shaderProgram.LinkProgram();

			if (shaderProgram.IsLinked())
				shaderProgram.UseProgram(true);

			mEngine.GetRenderer()->DrawTriangle(tri.vecA, tri.vecB, tri.vecC);

			while (true)
			{
				mEngine.Tick();
			}
		}

		mEngine.Destroy();
	}
}
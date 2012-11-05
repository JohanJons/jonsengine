#include "include/Game.h"
#include "include/Shaders.h"

#include "include/Video/OpenGL/Shader.h"
#include "include/Video/OpenGL/ShaderProgram.h"
#include "include/Core/Utils/Types.h"
#include "include/Input/InputManager.h"

#include "boost/bind.hpp"
#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace JonsEngine;

namespace JonsGame
{
	Game::Game() : mEngine(new Engine(mSettings))
	{
	}
		
	Game::~Game()
	{
        delete mEngine;
	}

		
	void Game::Run()
	{
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

			mEngine->GetRenderer().DrawTriangle(tri.vecA, tri.vecB, tri.vecC);

            mEngine->GetInputManager().RegisterMouseButtonCallback(boost::bind(&Game::OnMouseButtonEvent, this, _1));
            mEngine->GetInputManager().RegisterMouseMotionCallback(boost::bind(&Game::OnMouseMotionEvent, this, _1));
            mEngine->GetInputManager().RegisterKeyCallback(boost::bind(&Game::OnKeyEvent, this, _1));
            
			while (true)
			{
				mEngine->Tick();
			}
		}
	}

    void Game::OnKeyEvent(const KeyEvent& evnt)
    {
        
    }

    void Game::OnMouseButtonEvent(const JonsEngine::MouseButtonEvent& evnt)
    {

    }
        
    void Game::OnMouseMotionEvent(const JonsEngine::MouseMotionEvent& evnt)
    {

    }
}
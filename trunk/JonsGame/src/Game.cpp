#include "include/Game.h"
#include "include/Shaders.h"

#include "include/Video/OpenGL/Shader.h"
#include "include/Video/OpenGL/ShaderProgram.h"
#include "include/Video/OpenGL/UniformBuffer.h"
#include "include/Core/Utils/Types.h"
#include "include/Core/Utils/Math.h"
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
    Game::Game() : mEngine(new Engine(mSettings)), mRunning(true)
    {
    }
        
    Game::~Game()
    {
        delete mEngine;
    }

        
    void Game::Run()
    {
        SetupInputCallbacks();
        TestRendering();

        while (mRunning)
        {
            mEngine->Tick();
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

    void Game::SetupInputCallbacks()
    {
        mEngine->GetInputManager().RegisterMouseButtonCallback(boost::bind(&Game::OnMouseButtonEvent, this, _1));
        mEngine->GetInputManager().RegisterMouseMotionCallback(boost::bind(&Game::OnMouseMotionEvent, this, _1));
        mEngine->GetInputManager().RegisterKeyCallback(boost::bind(&Game::OnKeyEvent, this, _1));
    }
        
    void Game::TestRendering()
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

            shaderProgram.BindAttribLocation(0, "in_position");

            shaderProgram.LinkProgram();

            ShaderData shaderData;
            shaderData.mColor.x = 1.0f; shaderData.mColor.y = 1.0f; shaderData.mColor.z = 1.0f; shaderData.mColor.w = 1.0f;
            shaderData.mOffset.x = 0.5f; shaderData.mOffset.y = 0.5f;

            uint16_t h = mEngine->GetRenderer().GetScreenMode().ScreenHeight;
            uint16_t w = mEngine->GetRenderer().GetScreenMode().ScreenWidth;
            shaderData.mPerspMatrix = CreatePerspectiveMatrix(45.0f, w/(float) h, 0.5f, 5.0f);
            //shaderProgram.SetUniformBuffer(uni,"Uni");
            UniformBuffer uniBuffer("Uni");
            uniBuffer.SetData(shaderData);

            if (shaderProgram.IsLinked())
                shaderProgram.UseProgram(true);

            mEngine->GetRenderer().DrawTriangle(tri.vecA, tri.vecB, tri.vecC);
        }
    }
}
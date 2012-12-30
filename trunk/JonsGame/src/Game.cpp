#include "include/Game.h"
#include "include/Shaders.h"

#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/ShaderProgram.h"
#include "include/Core/Utils/Types.h"
#include "include/Core/Utils/Math.h"
#include "include/Input/InputManager.h"
#include "include/Scene/Mesh.h"
#include "include/Scene/Scene.h"

#include "boost/bind.hpp"
#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;
using namespace JonsEngine;

namespace JonsGame
{
    Game::Game() : mEngine(new Engine(mSettings)), mRunning(true), mUniBuffer("Uni")
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
        SetupScene();

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
            shaderData.mColor.x = 1.0f; shaderData.mColor.y = 0.5f; shaderData.mColor.z = 0.5f; shaderData.mColor.w = 1.0f;


            uint16_t h = mEngine->GetWindow().GetScreenMode().ScreenHeight;
            uint16_t w = mEngine->GetWindow().GetScreenMode().ScreenWidth;
            shaderData.mPerspMatrix = CreatePerspectiveMatrix(45.0f, w/(float) h, 0.5f, 10.0f);

            mUniBuffer.SetData(shaderData);

            if (shaderProgram.IsLinked())
                shaderProgram.UseProgram(true);

            shaderProgram.UseUniform(mUniBuffer, true);
        }
    }

    void Game::SetupScene()
    {
        const float vertexPositions1[] =
        {
            // front vertices
            -1.0f, -1.5f, -5.0f,
            -1.5f, -1.5f, -5.0f,
            -1.5f, -1.0f, -5.0f,
            -1.0f, -1.0f, -5.0f,

            // back vertices
            -1.0f, -1.5f, -5.5f,
            -1.5f, -1.5f, -5.5f,
            -1.5f, -1.0f, -5.5f,
            -1.0f, -1.0f, -5.5f
        };
 

        const uint16_t indexData1[] =
        {
            // back
            5, 4, 7,
            5, 7, 6,

            // right
            1, 5, 6,
            1, 6, 2,

            // left
            0, 4, 7,
            0, 7, 3,

            // top
            5, 4, 0,
            5, 0, 1,

            // bottom
            6, 7, 3,
            6, 3, 2,

            // front
            1, 0, 3,
            1, 3, 2
        };


        Scene* myScene = mEngine->GetSceneManager().CreateScene("MyScene");
        mEngine->GetSceneManager().SetActiveScene("MyScene");

        SceneNode* cube1 = myScene->GetRootNode().CreateChildNode("Cube1");
        cube1->SetMesh(Mesh::CreateMesh("Cube1Mesh", mEngine->GetRenderer().CreateVertexBuffer(vertexPositions1, sizeof(vertexPositions1)/sizeof(float), indexData1, sizeof(indexData1)/sizeof(uint16_t))));
    }
}
#include "include/Game.h"

#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/ShaderProgram.h"
#include "include/Core/Types.h"
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
        SetupScene();

        while (mRunning)
        {
            mEngine->Tick();
        }
    }

    void Game::OnKeyEvent(const KeyEvent& evnt)
    {
        SceneNode* cube = NULL;

        if (evnt.State == KeyEvent::PRESSED && (cube = mEngine->GetSceneManager().GetActiveScene()->GetRootNode().FindChildNode("Cube1")))
        {
            if (evnt.KeySymbol == A)
                cube->Translate(Vec3(-0.1f, 0.0f, 0.0f));

            if (evnt.KeySymbol == W)
                cube->Translate(Vec3(0.0f, 0.1f, 0.0f));

            if (evnt.KeySymbol == S)
                cube->Translate(Vec3(0.0f, -0.1f, 0.0f));

            if (evnt.KeySymbol == D)
                cube->Translate(Vec3(0.1f, 0.0f, 0.0f));

            if (evnt.KeySymbol == Q)
                cube->Scale(Vec3(2.0f, 1.0f, 1.0f));

            if (evnt.KeySymbol == E)
                cube->Scale(Vec3(0.5f, 1.0f, 1.0f));

            if (evnt.KeySymbol == R)
                cube->Rotate(25.0f, Vec3(1.0f, 0.0f, 0.0f));

            if (evnt.KeySymbol == T)
                cube->Rotate(25.0f, Vec3(0.0f, 1.0f, 0.0f));
        }
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

    void Game::SetupScene()
    {
        const float vertexPositions1[] =
        {
            // front vertices
            1.0f, -1.0f, 1.0f,           // bottom right
            -1.0f, -1.0f, 1.0f,          // bottom left
            -1.0f, 1.0f, 1.0f,          // top left
            1.0f, 1.0f, 1.0f,            // top right
 
            // back vertices
            1.0f, -1.0f, -1.0f,           // bottom right
            -1.0f, -1.0f, -1.0f,          // bottom left
            -1.0f, 1.0f, -1.0f,          // top left
            1.0f, 1.0f, -1.0f,            // top right
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
        SceneNode* cube2 = cube1->CreateChildNode("Cube2");
        cube2->SetMesh(Mesh::CreateMesh("Cube2Mesh", mEngine->GetRenderer().CreateVertexBuffer(vertexPositions1, sizeof(vertexPositions1)/sizeof(float), indexData1, sizeof(indexData1)/sizeof(uint16_t))));

        cube1->Translate(Vec3(5.0f, 0.0f, -15.0f));
        cube2->Translate(Vec3(0.0f, 2.0f, 0.0f));
        cube2->Scale(Vec3(2.0f, 1.0f, 1.0f));
    }
}
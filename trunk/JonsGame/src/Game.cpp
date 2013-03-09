#include "include/Game.h"

#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/ShaderProgram.h"
#include "include/Core/Types.h"
#include "include/Core/Utils/Math.h"
#include "include/Input/InputManager.h"
#include "include/Scene/Mesh.h"
#include "include/Scene/Scene.h"
#include "include/Resources/ResourceManifest.h"

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
        Scene* activeScene = mEngine->GetSceneManager().GetActiveScene();

        if (evnt.State == KeyEvent::PRESSED && (cube = activeScene->GetRootNode().FindChildNode("Node1")))
        {
            // transforming object
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


            // move camera
            if (evnt.KeySymbol == LEFT)
            {
                activeScene->GetSceneCamera().mCameraPosition -= Vec3(0.1f, 0.0f, 0.0f);
                activeScene->GetSceneCamera().mTargetVector -= Vec3(0.1f, 0.0f, 0.0f);
            }

            if (evnt.KeySymbol == RIGHT)
            {
                activeScene->GetSceneCamera().mCameraPosition += Vec3(0.1f, 0.0f, 0.0f);
                activeScene->GetSceneCamera().mTargetVector += Vec3(0.1f, 0.0f, 0.0f);
            }

            if (evnt.KeySymbol == UP)
            {
                activeScene->GetSceneCamera().mCameraPosition -= Vec3(0.0f, 0.0f, 0.1f);
                activeScene->GetSceneCamera().mTargetVector -= Vec3(0.0f, 0.0f, 0.1f);
            }

            if (evnt.KeySymbol == DOWN)
            {
                activeScene->GetSceneCamera().mCameraPosition += Vec3(0.0f, 0.0f, 0.1f);
                activeScene->GetSceneCamera().mTargetVector += Vec3(0.0f, 0.0f, 0.1f);
            }
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
        Scene* myScene = mEngine->GetSceneManager().CreateScene("MyScene");
        mEngine->GetSceneManager().SetActiveScene("MyScene");

        JonsPackagePtr package = ReadJonsPkg("../JonsEngine/bin/Debug/Win32/keke.jons");
        ModelPtr cubeModel = mEngine->GetResourceManifest().LoadModel("Cube", package);
        SceneNode* cubeNode = myScene->GetRootNode().CreateChildNode("Node1");
        SceneNode* cubeNode2 = myScene->GetRootNode().CreateChildNode("Node2");
        EntityPtr cubeEntity = myScene->CreateEntity("DasCube");
        EntityPtr cubeEntity2 = myScene->CreateEntity("DasCube2");

        cubeEntity->mModel = cubeModel;
        cubeEntity->mNode = cubeNode;
        cubeEntity2->mModel = cubeModel;
        cubeEntity2->mNode = cubeNode2;

        cubeNode->Translate(Vec3(5.0f, 0.0f, -10.0f));
        cubeNode2->Translate(Vec3(0.0f, 0.0f, -4.0f));
    }
}
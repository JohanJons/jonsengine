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
        SceneNodePtr cube;
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

        ModelPtr modelCube = mEngine->GetResourceManifest().LoadModel("cube", package);
        ModelPtr modelChair = mEngine->GetResourceManifest().LoadModel("chair", package);
        ModelPtr modelShotgun = mEngine->GetResourceManifest().LoadModel("shotgun", package);

        SceneNodePtr nodeCube = myScene->GetRootNode().CreateChildNode("Node1");
        SceneNodePtr nodeChair = myScene->GetRootNode().CreateChildNode("Node2");
        SceneNodePtr nodeShotgun = myScene->GetRootNode().CreateChildNode("Node3");

        EntityPtr entityCube = myScene->CreateEntity("DasCube");
        EntityPtr entityChair = myScene->CreateEntity("DasChair");
        EntityPtr entityShotgun = myScene->CreateEntity("DasShotgun");

        entityCube->mModel  = modelCube;
        entityCube->mNode   = nodeCube;
        entityChair->mModel = modelChair;
        entityChair->mNode  = nodeChair;
        entityShotgun->mModel  = modelShotgun;
        entityShotgun->mNode   = nodeShotgun;

        nodeCube->Translate(Vec3(7.0f, 0.0f, -15.0f));
        nodeChair->Translate(Vec3(0.0f, 0.0f, -8.0f));
        nodeShotgun->Translate(Vec3(0.0f, 0.0f, -4.0f));
        nodeShotgun->Scale(Vec3(0.1f, 0.1f, 0.1f));
        nodeShotgun->Rotate(90.0f, Vec3(0.0f, 0.0f, -1.0f));
        nodeShotgun->Rotate(270.0f, Vec3(0.0f, 1.0f, 0.0f));
    }
}
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
    Game::Game() : mEngine(new Engine(mSettings)), mRunning(true), mCenterYPos(mEngine->GetWindow().GetScreenMode().ScreenHeight/2), mCenterXPos(mEngine->GetWindow().GetScreenMode().ScreenWidth/2), mMoveSpeed(0.1f)
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

        mEngine->GetInputManager().ShowMouseCursor(false);
        mEngine->GetWindow().SetScreenResolution(1280, 720);

        while (mRunning)
        {
            mEngine->GetInputManager().SetMousePosition(mCenterXPos,mCenterYPos);

            mEngine->Tick();
        }
    }

    void Game::OnKeyEvent(const KeyEvent& evnt)
    {
        Scene* activeScene = mEngine->GetSceneManager().GetActiveScene();
        Camera& camera = activeScene->GetSceneCamera();

        if (evnt.State == KeyEvent::PRESSED)
        {
            // camera position
            if (evnt.KeySymbol == A)
                camera.TranslateCamera(-camera.Right() * mMoveSpeed);

            else if (evnt.KeySymbol == W)
                camera.TranslateCamera(camera.Forward() * mMoveSpeed);

            else if (evnt.KeySymbol == S)
                camera.TranslateCamera(-camera.Forward() * mMoveSpeed);

            else if (evnt.KeySymbol == D)
                camera.TranslateCamera(camera.Right() * mMoveSpeed);


            // light direction 
            else if (evnt.KeySymbol == Q)
                activeScene->GetLight("light1")->mLightDirection += Vec3(-0.05, 0.0f, 0.0f);

            else if (evnt.KeySymbol == E)
                activeScene->GetLight("light1")->mLightDirection += Vec3(0.05, 0.0f, 0.0f);

            else if (evnt.KeySymbol == R)
                activeScene->GetLight("light1")->mLightDirection += Vec3(0.00, -0.05f, 0.0f);

            else if (evnt.KeySymbol == T)
                activeScene->GetLight("light1")->mLightDirection += Vec3(0.0, 0.05f, 0.0f);

            else if (evnt.KeySymbol == F)
                activeScene->GetLight("light1")->mLightDirection += Vec3(0.0, 0.0f, -0.05f);

            else if (evnt.KeySymbol == G)
                activeScene->GetLight("light1")->mLightDirection += Vec3(0.0, 0.0f, 0.05f);
        }
    }

    void Game::OnMouseButtonEvent(const JonsEngine::MouseButtonEvent& evnt)
    {

    }
        
    void Game::OnMouseMotionEvent(const JonsEngine::MouseMotionEvent& evnt)
    {
        Scene* activeScene = mEngine->GetSceneManager().GetActiveScene();
        Camera& camera = activeScene->GetSceneCamera();

        const float sens = 0.1f;
        float newXPos = -((float)mCenterXPos - (float)evnt.PosX) * sens;
        float newYPos = -((float)mCenterYPos - (float)evnt.PosY) * sens;
        camera.RotateCamera(newXPos, newYPos);
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

        SceneNodePtr nodeCube    = myScene->GetRootNode().CreateChildNode("Node1");
        SceneNodePtr nodeChair   = myScene->GetRootNode().CreateChildNode("Node2");
        SceneNodePtr nodeShotgun = myScene->GetRootNode().CreateChildNode("Node3");
        SceneNodePtr nodeLight   = myScene->GetRootNode().CreateChildNode("Node4");
 
        EntityPtr entityCube    = myScene->CreateEntity("DasCube");
        EntityPtr entityChair   = myScene->CreateEntity("DasChair");
        EntityPtr entityShotgun = myScene->CreateEntity("DasShotgun");
        EntityPtr entityLight   = myScene->CreateEntity("DasLight");
 
        entityCube->mModel    = mEngine->GetResourceManifest().LoadModel("cube", package);
        entityCube->mNode     = nodeCube;
        entityChair->mModel   = mEngine->GetResourceManifest().LoadModel("chair", package);
        entityChair->mNode    = nodeChair;
        entityShotgun->mModel = mEngine->GetResourceManifest().LoadModel("shotgun", package);
        entityShotgun->mNode  = nodeShotgun;
        entityLight->mNode    = nodeLight;
        entityLight->mLight   = myScene->CreateLight("light1");
 
        entityLight->mLight->mLightDirection = Vec3(-0.8f, 0.0f, -1.0f);
        myScene->SetAmbientLight(Vec4(0.0f, 0.0f, 0.0f, 1.0f));

        nodeCube->TranslateNode(Vec3(7.0f, 0.0f, -15.0f));
        nodeChair->TranslateNode(Vec3(0.0f, 0.0f, -8.0f));
        nodeShotgun->TranslateNode(Vec3(0.0f, 0.0f, -4.0f));
    }
}
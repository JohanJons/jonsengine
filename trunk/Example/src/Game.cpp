#include "include/Game.h"

#include "include/Core/Types.h"
#include "include/Core/Utils/Math.h"
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
    Game::Game() : mEngine(new Engine(mSettings)), mRunning(true), mCenterYPos(mEngine->GetWindow().GetScreenHeight()/2), mCenterXPos(mEngine->GetWindow().GetScreenWidth()/2), mMoveSpeed(0.1f)
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

        mEngine->GetWindow().ShowMouseCursor(false);

        while (mRunning)
        {
            mEngine->GetWindow().SetMousePosition(mCenterXPos,mCenterYPos);

            mEngine->Tick(mDebugOptions);
        }
    }

    void Game::OnKeyEvent(const KeyEvent& evnt)
    {
        Scene* activeScene = mEngine->GetSceneManager().GetActiveScene();
        Camera& camera = activeScene->GetSceneCamera();

        if (evnt.mState == KeyEvent::STATE_PRESSED || evnt.mState == KeyEvent::STATE_REPEAT)
        {
            // camera position
            if (evnt.mKey == A)
                camera.TranslateCamera(-camera.Right() * mMoveSpeed);
            else if (evnt.mKey == W)
                camera.TranslateCamera(camera.Forward() * mMoveSpeed);
            else if (evnt.mKey == S)
                camera.TranslateCamera(-camera.Forward() * mMoveSpeed);
            else if (evnt.mKey == D)
                camera.TranslateCamera(camera.Right() * mMoveSpeed);

            // moving the point light 
            else if (evnt.mKey == Q)
                activeScene->GetPointLight("MovingPointLight")->mSceneNode->TranslateNode(Vec3(-0.05, 0.0f, 0.0f));
            else if (evnt.mKey == E)
                activeScene->GetPointLight("MovingPointLight")->mSceneNode->TranslateNode(Vec3(0.05, 0.0f, 0.0f));
            else if (evnt.mKey == R)
                activeScene->GetPointLight("MovingPointLight")->mSceneNode->TranslateNode(Vec3(0.00, -0.05f, 0.0f));
            else if (evnt.mKey == T)
                activeScene->GetPointLight("MovingPointLight")->mSceneNode->TranslateNode(Vec3(0.0, 0.05f, 0.0f));
            else if (evnt.mKey == F)
                activeScene->GetPointLight("MovingPointLight")->mSceneNode->TranslateNode(Vec3(0.0, 0.0f, -0.05f));
            else if (evnt.mKey == G)
                activeScene->GetPointLight("MovingPointLight")->mSceneNode->TranslateNode(Vec3(0.0, 0.0f, 0.05f));

            //  renderering
            else if (evnt.mKey == Y)
                mDebugOptions.mRenderingMode = DebugOptions::RENDER_MODE_FULL;
            else if (evnt.mKey == U)
                mDebugOptions.mRenderingMode = DebugOptions::RENDER_MODE_POSITIONS;
            else if (evnt.mKey == I)
                mDebugOptions.mRenderingMode = DebugOptions::RENDER_MODE_NORMALS;
            else if (evnt.mKey == O)
                mDebugOptions.mRenderingMode = DebugOptions::RENDER_MODE_DIFFUSE;
            else if (evnt.mKey == P)
                mDebugOptions.mRenderingFlags.flip(DebugOptions::RENDER_FLAG_DRAW_LIGHTS);
            else if (evnt.mKey == H)
                mEngine->GetRenderer()->SetAnisotropicFiltering(1.0f);
            else if (evnt.mKey == J)
                mEngine->GetRenderer()->SetAnisotropicFiltering(mEngine->GetRenderer()->GetMaxAnisotropicFiltering());

            // misc
            else if (evnt.mKey == N)
                mEngine->GetWindow().SetFullscreen(true);
            else if (evnt.mKey == M)
                mEngine->GetWindow().SetFullscreen(false);
            else if (evnt.mKey == ESC)
                mRunning = false;
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
        float newXPos = -((float)mCenterXPos - (float)evnt.mPosX) * sens;
        float newYPos = -((float)mCenterYPos - (float)evnt.mPosY) * sens;
        camera.RotateCamera(newXPos, newYPos);
    }

    void Game::SetupInputCallbacks()
    {
        mEngine->GetWindow().SetMouseButtonCallback(boost::bind(&Game::OnMouseButtonEvent, this, _1));
        mEngine->GetWindow().SetMouseMotionCallback(boost::bind(&Game::OnMouseMotionEvent, this, _1));
        mEngine->GetWindow().SetKeyCallback(boost::bind(&Game::OnKeyEvent, this, _1));
    }

    void Game::SetupScene()
    {
        Scene* myScene = mEngine->GetSceneManager().CreateScene("MyScene");
        mEngine->GetSceneManager().SetActiveScene("MyScene");
        JonsPackagePtr jonsPackage = ReadJonsPkg("../JonsEngine/bin/Debug/Win32/assets.jons");

        // chair
        SceneNodePtr nodeChair = myScene->GetRootNode().CreateChildNode("nodeChair");
        ModelPtr modelChair    = myScene->GetResourceManifest().LoadModel("chair", jonsPackage);
        modelChair->mSceneNode = nodeChair;
        nodeChair->TranslateNode(Vec3(0.0f, 0.0f, -8.0f));

         // cube
        SceneNodePtr nodeCube = myScene->GetRootNode().CreateChildNode("nodeCube");
        ModelPtr modelCube    = myScene->GetResourceManifest().LoadModel("cube", jonsPackage);
        modelCube->mSceneNode = nodeCube;
        nodeCube->TranslateNode(Vec3(7.0f, 1.0f, -15.0f));
        
        // uhura
        /*SceneNodePtr nodeUhura = myScene->GetRootNode().CreateChildNode("nodeUhura");
        ModelPtr modelUhura    = myScene->GetResourceManifest().LoadModel("uhura", jonsPackage);
        modelUhura->mSceneNode = nodeUhura;
        nodeUhura->TranslateNode(Vec3(0.0f, 0.0f, -4.0f));*/
        
        // point light
        SceneNodePtr nodeMovingLight = myScene->GetRootNode().CreateChildNode("nodeMovingLight");
        PointLightPtr movingLight    = myScene->CreatePointLight("MovingPointLight", nodeMovingLight);
        movingLight->mMaxDistance    = 4.0f;
        movingLight->mLightColor     = Vec4(1.0f, 1.0f, 0.0f, 0.0f);
        nodeMovingLight->TranslateNode(Vec3(4.0f, 0.5f, -15.0f));

        // directional light
        SceneNodePtr nodeDirLight = myScene->GetRootNode().CreateChildNode("nodeDirLight");
        DirectionalLightPtr directionalLight = myScene->CreateDirectionalLight("DirectionalLight");
        directionalLight->mLightDirection = Vec3(0.5f, 0.5f, 0.0f);
        directionalLight->mLightColor = Vec4(0.5f);

        // point light 2
       /* SceneNodePtr nodeMovingLight2 = myScene->GetRootNode().CreateChildNode("nodeStaticLight");
        LightPtr staticLight          = myScene->CreateLight("StaticPointLight", LightType::LIGHT_TYPE_POINT, nodeMovingLight2);
        staticLight->mLightColor      = Vec4(0.4f, 0.4f, 0.0f, 1.0f);
        staticLight->mRadius          = 5.0f;
        nodeMovingLight2->TranslateNode(Vec3(0.0f, 3.0f, -8.0f));*/

        // ambient light
        myScene->SetAmbientLight(Vec4(0.1f));
        
        // create a ground plane
        SceneNodePtr nodePlane       = myScene->GetRootNode().CreateChildNode("nodePlane");
        ModelPtr plane               = myScene->GetResourceManifest().CreateRectangle("GroundPlane", 64, 0.5, 64);
        plane->mMaterial             = myScene->GetResourceManifest().LoadMaterial("checker", jonsPackage);
        plane->mSceneNode            = nodePlane;
        plane->mMaterialTilingFactor = 64.0f;
        nodePlane->TranslateNode(Vec3(0.0f, -0.5f, 0.0f));

        // create a sphere
        SceneNodePtr nodeSphere = myScene->GetRootNode().CreateChildNode("nodeSphere");
        ModelPtr sphere = myScene->GetResourceManifest().CreateSphere("Sphere", 1.0f, 12, 24);
        sphere->mMaterial = myScene->GetResourceManifest().GetMaterial("checker");
        sphere->mSceneNode = nodeSphere;
        nodeSphere->TranslateNode(Vec3(6.0f, 5.5f, 10.0f));
        
        // move up camera
        myScene->GetSceneCamera().TranslateCamera(Vec3(0.0f, 3.0f, 0.0f));

        // set anisotropic filter to max
        float maxAnisoLevel = mEngine->GetRenderer()->GetMaxAnisotropicFiltering();
        mEngine->GetRenderer()->SetAnisotropicFiltering(maxAnisoLevel);
    }
}
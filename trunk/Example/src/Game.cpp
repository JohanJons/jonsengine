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
    Game::Game() : mEngine(new Engine(mSettings)), mRunning(true), mSunAngle(0.0f), mMoveSpeed(0.1f)
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
        mEngine->GetWindow().SetFrameLimit(60);

        while (mRunning)
        {
            UpdateSun();

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
            if (evnt.mKey == Key::A)
                camera.TranslateCamera(-camera.Right() * mMoveSpeed);
            else if (evnt.mKey == Key::W)
                camera.TranslateCamera(camera.Forward() * mMoveSpeed);
            else if (evnt.mKey == Key::S)
                camera.TranslateCamera(-camera.Forward() * mMoveSpeed);
            else if (evnt.mKey == Key::D)
                camera.TranslateCamera(camera.Right() * mMoveSpeed);

            // moving the point light 
            else if (evnt.mKey == Key::Q)
                activeScene->GetPointLight("MovingPointLight")->mSceneNode->TranslateNode(Vec3(-0.05, 0.0f, 0.0f));
            else if (evnt.mKey == Key::E)
                activeScene->GetPointLight("MovingPointLight")->mSceneNode->TranslateNode(Vec3(0.05, 0.0f, 0.0f));
            else if (evnt.mKey == Key::R)
                activeScene->GetPointLight("MovingPointLight")->mSceneNode->TranslateNode(Vec3(0.00, -0.05f, 0.0f));
            else if (evnt.mKey == Key::T)
                activeScene->GetPointLight("MovingPointLight")->mSceneNode->TranslateNode(Vec3(0.0, 0.05f, 0.0f));
            else if (evnt.mKey == Key::F)
                activeScene->GetPointLight("MovingPointLight")->mSceneNode->TranslateNode(Vec3(0.0, 0.0f, -0.05f));
            else if (evnt.mKey == Key::G)
                activeScene->GetPointLight("MovingPointLight")->mSceneNode->TranslateNode(Vec3(0.0, 0.0f, 0.05f));

            //  renderering
            else if (evnt.mKey == Key::ONE)
                mDebugOptions.mRenderingMode = DebugOptions::RENDER_MODE_FULL;
            else if (evnt.mKey == Key::TWO)
                mDebugOptions.mRenderingMode = DebugOptions::RENDER_MODE_POSITIONS;
            else if (evnt.mKey == Key::THREE)
                mDebugOptions.mRenderingMode = DebugOptions::RENDER_MODE_NORMALS;
            else if (evnt.mKey == Key::FOUR)
                mDebugOptions.mRenderingMode = DebugOptions::RENDER_MODE_DIFFUSE;
            else if (evnt.mKey == Key::FIVE)
                mDebugOptions.mRenderingMode = DebugOptions::RENDER_MODE_DEPTH;
            else if (evnt.mKey == Key::P)
                mDebugOptions.mRenderingFlags.flip(DebugOptions::RENDER_FLAG_DRAW_LIGHTS);
            else if (evnt.mKey == Key::O)
                mDebugOptions.mRenderingFlags.flip(DebugOptions::RENDER_FLAG_SHADOWMAP_SPLITS);
            else if (evnt.mKey == Key::H)
                mEngine->GetRenderer()->SetAnisotropicFiltering(EngineSettings::ANISOTROPIC_1X);
            else if (evnt.mKey == Key::J)
                mEngine->GetRenderer()->SetAnisotropicFiltering(EngineSettings::ANISOTROPIC_16X);
            else if (evnt.mKey == Key::K)
                mEngine->GetRenderer()->SetMSAA(EngineSettings::MSAA_1X);
            else if (evnt.mKey == Key::L)
                mEngine->GetRenderer()->SetMSAA(EngineSettings::MSAA_4X);

            // misc
            else if (evnt.mKey == Key::N)
                mEngine->GetWindow().SetFullscreen(true);
            else if (evnt.mKey == Key::M)
                mEngine->GetWindow().SetFullscreen(false);
            else if (evnt.mKey == Key::ESC)
                mRunning = false;
        }
    }

    void Game::OnMouseButtonEvent(const JonsEngine::MouseButtonEvent& evnt)
    {
    }
        
    void Game::OnMousePositionEvent(const JonsEngine::MousePositionEvent& evnt)
    {
        Scene* activeScene = mEngine->GetSceneManager().GetActiveScene();
        Camera& camera = activeScene->GetSceneCamera();

        const float sens = 0.1f;
        float newXPos = -((float)evnt.mPreviousPosX - (float)evnt.mCurrentPosX) * sens;
        float newYPos = -((float)evnt.mPreviousPosY - (float)evnt.mCurrentPosY) * sens;

        camera.RotateCamera(newXPos, newYPos);
    }

    void Game::SetupInputCallbacks()
    {
        mEngine->GetWindow().SetMouseButtonCallback(boost::bind(&Game::OnMouseButtonEvent, this, _1));
        mEngine->GetWindow().SetMousePositionCallback(boost::bind(&Game::OnMousePositionEvent, this, _1));
        mEngine->GetWindow().SetKeyCallback(boost::bind(&Game::OnKeyEvent, this, _1));
    }

    void Game::SetupScene()
    {
        Scene* myScene = mEngine->GetSceneManager().CreateScene("MyScene");
        mEngine->GetSceneManager().SetActiveScene("MyScene");
        JonsPackagePtr jonsPackage = ReadJonsPkg("../JonsEngine/bin/Debug/Win32/assets.jons");

        // sectoid
        SceneNodePtr nodeAlien = myScene->GetRootNode().CreateChildNode("nodeSectoid");
        ModelPtr modelAlien    = myScene->GetResourceManifest().LoadModel("sectoid", jonsPackage);
        modelAlien->mSceneNode = nodeAlien;
        nodeAlien->RotateNode(90.0f, Vec3(1.0f, 0.0f, 0.0f));
        nodeAlien->TranslateNode(Vec3(0.0f, 0.5f, -4.0f));

         // cube
        SceneNodePtr nodeCube = myScene->GetRootNode().CreateChildNode("nodeCube");
        ModelPtr modelCube    = myScene->GetResourceManifest().LoadModel("cube", jonsPackage);
        modelCube->mSceneNode = nodeCube;
        nodeCube->TranslateNode(Vec3(7.0f, 1.0f, -15.0f));
        
        // ninja
        /*SceneNodePtr nodeUhura = myScene->GetRootNode().CreateChildNode("uhura");
        ModelPtr modelUhura    = myScene->GetResourceManifest().LoadModel("uhura", jonsPackage);
        modelUhura->mSceneNode = nodeUhura;
        nodeUhura->TranslateNode(Vec3(-8.0f, 0.5f, -4.0f));
        nodeUhura->ScaleNode(Vec3(2.0f));*/
        
        // point light
        SceneNodePtr nodeMovingLight = myScene->GetRootNode().CreateChildNode("nodeMovingLight");
        PointLightPtr movingLight    = myScene->CreatePointLight("MovingPointLight", nodeMovingLight);
        movingLight->mMaxDistance    = 10.0f;
        movingLight->mLightIntensity = 2.0f;
        movingLight->mLightColor     = Vec4(1.0f, 1.0f, 0.0f, 0.0f);
        nodeMovingLight->TranslateNode(Vec3(5.0f, 3.5f, -15.0f));

        // directional light
        DirectionalLightPtr directionalLight = myScene->CreateDirectionalLight("DirectionalLight");
        directionalLight->mLightDirection = Vec3(-1.0f, -1.0f, -1.0f);
        directionalLight->mLightColor = Vec4(0.3f);

        // ambient light
        myScene->SetAmbientLight(Vec4(0.05f));
        
        // create a ground plane
        SceneNodePtr nodePlane       = myScene->GetRootNode().CreateChildNode("nodePlane");
        ModelPtr plane               = myScene->GetResourceManifest().CreateRectangle("GroundPlane", 64, 1.0, 64);
        plane->mMaterial             = myScene->GetResourceManifest().LoadMaterial("checker", jonsPackage);
        plane->mSceneNode            = nodePlane;
        plane->mMaterialTilingFactor = 64.0f;

        // create a sphere
        SceneNodePtr nodeSphere = myScene->GetRootNode().CreateChildNode("nodeSphere");
        ModelPtr sphere = myScene->GetResourceManifest().CreateSphere("Sphere", 1.0f, 12, 24);
        sphere->mMaterial = myScene->GetResourceManifest().GetMaterial("checker");
        sphere->mSceneNode = nodeSphere;
        nodeSphere->TranslateNode(Vec3(6.0f, 5.5f, 10.0f));
        
        // move up camera
        myScene->GetSceneCamera().TranslateCamera(Vec3(0.0f, 3.0f, 0.0f));
    }

    void Game::UpdateSun()
    {
        Scene* activeScene = mEngine->GetSceneManager().GetActiveScene();

       // mSunDirection += JonsEngine::Vec3(glm::sin(0.1f), 0.0f, 0.0f);
       // activeScene->GetDirectionalLight("DirectionalLight")->mLightDirection = mSunDirection;
        //activeScene->GetDirectionalLight("DirectionalLight")->mLightDirection += (Vec3(0.0, 0.1f, 0.0f));
        //activeScene->GetDirectionalLight("DirectionalLight")->mLightDirection -= (Vec3(0.0, 0.1f, 0.0f));
    }
}
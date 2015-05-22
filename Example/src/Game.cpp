#include "include/Game.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Types.h"
#include "include/Scene/Scene.h"
#include "include/Resources/ResourceManifest.h"

#include "boost/bind.hpp"

using namespace std;
using namespace JonsEngine;

/*
 * Demo/interactive test of JonsEngine features
 * Requires JonsEngine binary asset file (.jons) to load art resources - see AssetImporter project
 */

namespace JonsGame
{
    Game::Game() : mEngine(new Engine(mSettings)), mRunning(true), mSunAngle(0.0f), mMoveSpeed(0.1f)
    {
    }
        
    Game::~Game()
    {
    }

        
    void Game::Run()
    {
        SetupInputCallbacks();
        SetupScene();

        //mEngine->GetWindow().SetFullscreen(true);
        mEngine->GetWindow().ShowMouseCursor(false);

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

        if (evnt.mState == KeyEvent::KeyState::STATE_PRESSED && evnt.mShiftPressed)
        {
            //  renderering
            if (evnt.mKey == Key::ONE)
                mEngine->GetRenderer().SetShadowFiltering(EngineSettings::ShadowFiltering::PCF_2X2);
            else if (evnt.mKey == Key::TWO)
                mEngine->GetRenderer().SetShadowFiltering(EngineSettings::ShadowFiltering::PCF_3X3);
            else if (evnt.mKey == Key::THREE)
                mEngine->GetRenderer().SetShadowFiltering(EngineSettings::ShadowFiltering::PCF_5X5);
            else if (evnt.mKey == Key::FOUR)
                mEngine->GetRenderer().SetShadowFiltering(EngineSettings::ShadowFiltering::PCF_7X7);
        }
        else if (evnt.mState == KeyEvent::KeyState::STATE_PRESSED || evnt.mState == KeyEvent::KeyState::STATE_REPEAT)
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
                activeScene->GetPointLight(mPointLightID).mSceneNode->TranslateNode(Vec3(-0.05f, 0.0f, 0.0f));
            else if (evnt.mKey == Key::E)
                activeScene->GetPointLight(mPointLightID).mSceneNode->TranslateNode(Vec3(0.05f, 0.0f, 0.0f));
            else if (evnt.mKey == Key::R)
                activeScene->GetPointLight(mPointLightID).mSceneNode->TranslateNode(Vec3(0.0f, -0.05f, 0.0f));
            else if (evnt.mKey == Key::T)
                activeScene->GetPointLight(mPointLightID).mSceneNode->TranslateNode(Vec3(0.0f, 0.05f, 0.0f));
            else if (evnt.mKey == Key::F)
                activeScene->GetPointLight(mPointLightID).mSceneNode->TranslateNode(Vec3(0.0f, 0.0f, -0.05f));
            else if (evnt.mKey == Key::G)
                activeScene->GetPointLight(mPointLightID).mSceneNode->TranslateNode(Vec3(0.0f, 0.0f, 0.05f));

            //  renderering
            else if (evnt.mKey == Key::ONE)
                mDebugOptions.mRenderingFlags.flip(DebugOptions::RENDER_FLAG_DRAW_AABB);
            else if (evnt.mKey == Key::TWO)
                mDebugOptions.mRenderingFlags.flip(DebugOptions::RENDER_FLAG_SHADOWMAP_SPLITS);
            else if (evnt.mKey == Key::U)
                mEngine->GetRenderer().SetSSAO(false);
            else if (evnt.mKey == Key::I)
                mEngine->GetRenderer().SetSSAO(true);
            else if (evnt.mKey == Key::H)
                mEngine->GetRenderer().SetAnisotropicFiltering(EngineSettings::Anisotropic::X1);
            else if (evnt.mKey == Key::J)
                mEngine->GetRenderer().SetAnisotropicFiltering(EngineSettings::Anisotropic::X16);
            else if (evnt.mKey == Key::K)
                mEngine->GetRenderer().SetAntiAliasing(EngineSettings::AntiAliasing::NONE);
            else if (evnt.mKey == Key::L)
                mEngine->GetRenderer().SetAntiAliasing(EngineSettings::AntiAliasing::FXAA);

            // misc
            else if (evnt.mKey == Key::N)
                mEngine->GetWindow().SetFullscreen(true);
            else if (evnt.mKey == Key::M)
                mEngine->GetWindow().SetFullscreen(false);
            else if (evnt.mKey == Key::V)
                mEngine->GetWindow().ShowMouseCursor(true);
            else if (evnt.mKey == Key::B)
                mEngine->GetWindow().ShowMouseCursor(false);
            else if (evnt.mKey == Key::X)
                mEngine->GetWindow().SetScreenResolution(1080, 720);
            else if (evnt.mKey == Key::C)
                mEngine->GetWindow().SetScreenResolution(1920, 1080);
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
        float newXPos = (float)evnt.mRelativePosX * sens;
        float newYPos = (float)evnt.mRelativePosY * sens;

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
        Scene* myScene = mEngine->GetSceneManager().GetActiveScene();
        JonsPackagePtr jonsPackage = ReadJonsPkg("assets.jons");

        //SetupSponzaScene(myScene, jonsPackage);
        SetupTestScene(myScene, jonsPackage);
    }

    void Game::SetupTestScene(Scene* myScene, JonsPackagePtr jonsPackage)
    {
        // ambient light
        myScene->SetAmbientLight(Vec4(0.01f));

        // sectoid
        SceneNodePtr nodeAlien = myScene->GetRootNode().CreateChildNode("nodeSectoid");
        ModelPtr modelAlien = mEngine->GetResourceManifest().LoadModel("sectoid", jonsPackage);
        Actor* actorAlien = myScene->CreateActor("actorSectoid", modelAlien, nodeAlien);
        nodeAlien->TranslateNode(Vec3(0.0f, 0.5f, -4.0f));
        nodeAlien->RotateNode(90.0f, Vec3(1.0f, 0.0f, 0.0f));

        // cube
        SceneNodePtr nodeCube = myScene->GetRootNode().CreateChildNode("nodeCube");
        ModelPtr modelCube = mEngine->GetResourceManifest().LoadModel("cube", jonsPackage);
        Actor* actorCube = myScene->CreateActor("actorCube", modelCube, nodeCube);
        nodeCube->TranslateNode(Vec3(7.0f, 1.0f, -15.0f));

        // chair
        SceneNodePtr nodeChair = myScene->GetRootNode().CreateChildNode("nodeChair");
        ModelPtr modelChair = mEngine->GetResourceManifest().LoadModel("chair", jonsPackage);
        Actor* actorChair = myScene->CreateActor("actorChair", modelChair, nodeChair);
        nodeChair->TranslateNode(Vec3(-8.0f, 0.5f, -4.0f));
        nodeChair->ScaleNode(2.0f);

        // house
        SceneNodePtr nodeHouse = myScene->GetRootNode().CreateChildNode("nodeHouse");
        ModelPtr modelHouse = mEngine->GetResourceManifest().LoadModel("house", jonsPackage);
        Actor* actorHouse = myScene->CreateActor("actorHouse", modelHouse, nodeHouse);
        nodeHouse->TranslateNode(Vec3(-7.0f, 0.5f, -15.0f));

        // point light
        SceneNodePtr nodeMovingLight = myScene->GetRootNode().CreateChildNode("nodeMovingLight");
        mPointLightID = myScene->CreatePointLight("MovingPointLight", nodeMovingLight);
        PointLight& movingLight = myScene->GetPointLight(mPointLightID);
        movingLight.mLightRadius = 10.0f;
        movingLight.mLightIntensity = 2.0f;
        movingLight.mLightColor = Vec4(1.0f, 1.0f, 0.0f, 0.0f);
        nodeMovingLight->TranslateNode(Vec3(5.0f, 3.5f, -15.0f));

        // directional light
        DirectionalLight* directionalLight = myScene->CreateDirectionalLight("DirectionalLight");
        directionalLight->mLightDirection = Vec3(-1.0f, -1.0f, -1.0f);
        directionalLight->mLightColor = Vec4(0.55f);

        // load checker material
        MaterialPtr checkerMaterial = mEngine->GetResourceManifest().LoadMaterial("checkers", jonsPackage);

        // create a ground plane
        SceneNodePtr nodePlane = myScene->GetRootNode().CreateChildNode("nodePlane");
        ModelPtr plane = mEngine->GetResourceManifest().CreateRectangle("GroundPlane", 64, 1.0, 64, checkerMaterial);
		plane->GetRootNode().GetMeshes()[0].SetTextureTilingFactor(64.0f);
        Actor* actorPlane = myScene->CreateActor("actorPlane", plane, nodePlane);

        // create a sphere
        SceneNodePtr nodeSphere = myScene->GetRootNode().CreateChildNode("nodeSphere");
        ModelPtr sphere = mEngine->GetResourceManifest().CreateSphere("Sphere", 1.0f, 12, 24, checkerMaterial);
        Actor* actorSphere = myScene->CreateActor("actorHouse", sphere, nodeSphere);
        nodeSphere->TranslateNode(Vec3(6.0f, 5.5f, 10.0f));

        // create a  second cube
        SceneNodePtr nodeCube2 = myScene->GetRootNode().CreateChildNode("nodeCube2");
        ModelPtr cube2 = mEngine->GetResourceManifest().CreateCube("Cube2", 3, checkerMaterial);
        Actor* actorCube2 = myScene->CreateActor("actorCube2", cube2, nodeCube2);
        nodeCube2->TranslateNode(Vec3(3.0f, 2.0f, -15.0f));

        // create a third cube, far away casting shadows from the dir light
        SceneNodePtr nodeCube3 = myScene->GetRootNode().CreateChildNode("nodeCube3");
        ModelPtr cube3 = mEngine->GetResourceManifest().CreateCube("Cube3", 3, checkerMaterial);
        Actor* actorCube3 = myScene->CreateActor("actorCube3", cube3, nodeCube3);
        nodeCube3->TranslateNode(-directionalLight->mLightDirection * 40.0f);

        // move up camera
        myScene->GetSceneCamera().TranslateCamera(Vec3(0.0f, 3.0f, 0.0f));
    }

    void Game::SetupSponzaScene(Scene* myScene, JonsPackagePtr jonsPackage)
    {
        // ambient light
        myScene->SetAmbientLight(Vec4(0.1f));

        // directional light
        /*DirectionalLight* directionalLight = myScene->CreateDirectionalLight("DirectionalLight");
        directionalLight->mLightDirection = Vec3(-1.0f, -1.0f, -1.0f);
        directionalLight->mLightColor = Vec4(1.0F);*/
        
        mMoveSpeed = 1.0f;

        // sponza 
        SceneNodePtr nodeSponza = myScene->GetRootNode().CreateChildNode("sponza");
        ModelPtr modelSponza = mEngine->GetResourceManifest().LoadModel("sponza", jonsPackage);
        // this model is huge compared to others - scale it down massively
        nodeSponza->ScaleNode(0.05f);
        nodeSponza->TranslateNode(Vec3(0.0f, 0.5f, -54.0f));
        Actor* actorSponza = myScene->CreateActor("actorPlane", modelSponza, nodeSponza);
        
        // move up camera
        myScene->GetSceneCamera().TranslateCamera(Vec3(0.0f, 3.0f, 0.0f));
    }

    void Game::UpdateSun()
    {
        // TODO: animated dirlight as sun
    }
}
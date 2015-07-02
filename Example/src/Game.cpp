#include "include/Game.h"

#include "include/Core/Logging/Logger.h"
#include "include/Core/Types.h"
#include "include/Scene/Scene.h"
#include "include/Resources/ResourceManifest.h"

#include "boost/bind.hpp"
#include <chrono>

using namespace std;
using namespace JonsEngine;

/*
 * Demo/interactive test of JonsEngine features
 * Requires JonsEngine binary asset file (.jons) to load art resources - see AssetImporter project
 */

namespace JonsGame
{
    Game::Game() : mEngine(mSettings), mRunning(true), mMoveSpeed(0.1f), mPointLightID(0), mSunSpeed(0.0002f), mSunMoving(true)
    {
    }
        
    Game::~Game()
    {
    }

        
    void Game::Run()
    {
        SetupInputCallbacks();
        SetupScene();

        //mEngine.GetWindow().SetFullscreen(true);
        mEngine.GetWindow().ShowMouseCursor(false);

        while (mRunning)
        {
            if (mSunMoving)
                UpdateSun();

            mEngine.Tick(mDebugOptions);
        }
    }

    void Game::OnKeyEvent(const KeyEvent& evnt)
    {
        Scene* activeScene = mEngine.GetSceneManager().GetActiveScene();
        Camera& camera = activeScene->GetSceneCamera();

        // SHIFT-modifier activated
        if ((evnt.mState == KeyEvent::KeyState::STATE_PRESSED || evnt.mState == KeyEvent::KeyState::STATE_REPEAT) && evnt.mShiftPressed)
        {
            switch (evnt.mKey)
            {
                //  renderering
                case Key::ONE: mEngine.GetRenderer().SetShadowFiltering(EngineSettings::ShadowFiltering::PCF_2X2); break;
                case Key::TWO: mEngine.GetRenderer().SetShadowFiltering(EngineSettings::ShadowFiltering::PCF_3X3); break;
                case Key::THREE: mEngine.GetRenderer().SetShadowFiltering(EngineSettings::ShadowFiltering::PCF_5X5); break;
                case Key::FOUR: mEngine.GetRenderer().SetShadowFiltering(EngineSettings::ShadowFiltering::PCF_7X7); break;

                // sun
                case Key::Q: mSunMoving = true; break;
                case Key::W: mSunMoving = false; break;
                case Key::E: mSunSpeed += 0.005f; break;
                case Key::R: mSunSpeed -= 0.005f; break;
                
                default:
                    break;
            }
        }
        // no modifiers
        else if (evnt.mState == KeyEvent::KeyState::STATE_PRESSED || evnt.mState == KeyEvent::KeyState::STATE_REPEAT)
        {
            switch (evnt.mKey)
            {
                // camera
                case Key::A: camera.TranslateCamera(-camera.Right() * mMoveSpeed);   break;
                case Key::W: camera.TranslateCamera(camera.Forward() * mMoveSpeed);  break;
                case Key::S: camera.TranslateCamera(-camera.Forward() * mMoveSpeed); break;
                case Key::D: camera.TranslateCamera(camera.Right() * mMoveSpeed);    break;
                
                // moving the point light 
                case Key::Q: activeScene->GetPointLight(mPointLightID).mSceneNode->TranslateNode(Vec3(-0.05f, 0.0f, 0.0f)); break;
                case Key::E: activeScene->GetPointLight(mPointLightID).mSceneNode->TranslateNode(Vec3(0.05f, 0.0f, 0.0f));  break;
                case Key::R: activeScene->GetPointLight(mPointLightID).mSceneNode->TranslateNode(Vec3(0.0f, -0.05f, 0.0f)); break;
                case Key::T: activeScene->GetPointLight(mPointLightID).mSceneNode->TranslateNode(Vec3(0.0f, 0.05f, 0.0f));  break;
                case Key::F: activeScene->GetPointLight(mPointLightID).mSceneNode->TranslateNode(Vec3(0.0f, 0.0f, -0.05f)); break;
                case Key::G: activeScene->GetPointLight(mPointLightID).mSceneNode->TranslateNode(Vec3(0.0f, 0.0f, 0.05f));  break;

                //  renderering
                case Key::ONE: mDebugOptions.mRenderingFlags.flip(DebugOptions::RENDER_FLAG_DRAW_AABB); break;
                case Key::TWO: mDebugOptions.mRenderingFlags.flip(DebugOptions::RENDER_FLAG_SHADOWMAP_SPLITS); break;
                case Key::U: mEngine.GetRenderer().SetSSAO(false); break;
                case Key::I: mEngine.GetRenderer().SetSSAO(true); break;
                case Key::H: mEngine.GetRenderer().SetAnisotropicFiltering(EngineSettings::Anisotropic::X1); break;
                case Key::J: mEngine.GetRenderer().SetAnisotropicFiltering(EngineSettings::Anisotropic::X16); break;
                case Key::K: mEngine.GetRenderer().SetAntiAliasing(EngineSettings::AntiAliasing::NONE); break;
                case Key::L: mEngine.GetRenderer().SetAntiAliasing(EngineSettings::AntiAliasing::FXAA); break;

                //  misc
                case Key::N: mEngine.GetWindow().SetFullscreen(true); break;
                case Key::M: mEngine.GetWindow().SetFullscreen(false); break;
                case Key::V: mEngine.GetWindow().ShowMouseCursor(true); break;
                case Key::B: mEngine.GetWindow().ShowMouseCursor(false); break;
                case Key::X: mEngine.GetWindow().SetScreenResolution(1080, 720); break;
                case Key::C: mEngine.GetWindow().SetScreenResolution(1920, 1080); break;
                case Key::ESC: mRunning = false; break;

                default:
                    break;
            }
        }
    }

    void Game::OnMouseButtonEvent(const JonsEngine::MouseButtonEvent& evnt)
    {
    }

    void Game::OnMousePositionEvent(const JonsEngine::MousePositionEvent& evnt)
    {
        Scene* activeScene = mEngine.GetSceneManager().GetActiveScene();
        Camera& camera = activeScene->GetSceneCamera();

        const float sens = 0.1f;
        float newXPos = (float)evnt.mRelativePosX * sens;
        float newYPos = (float)evnt.mRelativePosY * sens;

        camera.RotateCamera(newXPos, newYPos);
    }

    void Game::SetupInputCallbacks()
    {
        mEngine.GetWindow().SetMouseButtonCallback(boost::bind(&Game::OnMouseButtonEvent, this, _1));
        mEngine.GetWindow().SetMousePositionCallback(boost::bind(&Game::OnMousePositionEvent, this, _1));
        mEngine.GetWindow().SetKeyCallback(boost::bind(&Game::OnKeyEvent, this, _1));
    }

    void Game::SetupScene()
    {
        JonsPackagePtr jonsPackage = ReadJonsPkg("assets.jons");

        //SetupSponzaScene(myScene, jonsPackage);
        SetupTestScene(jonsPackage);
    }

    void Game::SetupTestScene(JonsPackagePtr jonsPackage)
    {
        Scene* scene = mEngine.GetSceneManager().GetActiveScene();

        // ambient light
        scene->SetAmbientLight(Vec4(0.01f));

        // sectoid
        SceneNodePtr nodeAlien = scene->GetRootNode().CreateChildNode("nodeSectoid");
        ModelPtr modelAlien = mEngine.GetResourceManifest().LoadModel("sectoid", jonsPackage);
        Actor* actorAlien = scene->CreateActor("actorSectoid", modelAlien, nodeAlien);
        nodeAlien->TranslateNode(Vec3(0.0f, 0.5f, -4.0f));
        nodeAlien->RotateNode(90.0f, Vec3(1.0f, 0.0f, 0.0f));

        // cube
        SceneNodePtr nodeCube = scene->GetRootNode().CreateChildNode("nodeCube");
        ModelPtr modelCube = mEngine.GetResourceManifest().LoadModel("cube", jonsPackage);
        Actor* actorCube = scene->CreateActor("actorCube", modelCube, nodeCube);
        nodeCube->TranslateNode(Vec3(7.0f, 1.0f, -15.0f));

        // chair
        SceneNodePtr nodeChair = scene->GetRootNode().CreateChildNode("nodeChair");
        ModelPtr modelChair = mEngine.GetResourceManifest().LoadModel("chair", jonsPackage);
        Actor* actorChair = scene->CreateActor("actorChair", modelChair, nodeChair);
        nodeChair->TranslateNode(Vec3(-8.0f, 0.5f, -4.0f));
        nodeChair->ScaleNode(2.0f);

        // house
        SceneNodePtr nodeHouse = scene->GetRootNode().CreateChildNode("nodeHouse");
        ModelPtr modelHouse = mEngine.GetResourceManifest().LoadModel("house", jonsPackage);
        Actor* actorHouse = scene->CreateActor("actorHouse", modelHouse, nodeHouse);
        nodeHouse->TranslateNode(Vec3(-7.0f, 0.5f, -15.0f));

        // point light
        SceneNodePtr nodeMovingLight = scene->GetRootNode().CreateChildNode("nodeMovingLight");
        mPointLightID = scene->CreatePointLight("MovingPointLight", nodeMovingLight);
        PointLight& movingLight = scene->GetPointLight(mPointLightID);
        movingLight.mLightRadius = 10.0f;
        movingLight.mLightIntensity = 2.0f;
        movingLight.mLightColor = Vec4(1.0f, 1.0f, 0.0f, 0.0f);
        nodeMovingLight->TranslateNode(Vec3(5.0f, 3.5f, -15.0f));

        // directional light
        DirectionalLight* directionalLight = scene->CreateDirectionalLight("DirectionalLight", 4);
        directionalLight->mLightDirection = Vec3(-1.0f, -1.0f, -1.0f);
        directionalLight->mLightColor = Vec4(0.55f);

        // load checker material
        MaterialPtr checkerMaterial = mEngine.GetResourceManifest().LoadMaterial("checkers", jonsPackage);

        // create a ground plane
        SceneNodePtr nodePlane = scene->GetRootNode().CreateChildNode("nodePlane");
        ModelPtr plane = mEngine.GetResourceManifest().CreateRectangle("GroundPlane", 64, 1.0, 64, checkerMaterial);
		plane->GetRootNode().GetMeshes()[0].SetTextureTilingFactor(64.0f);
        Actor* actorPlane = scene->CreateActor("actorPlane", plane, nodePlane);

        // create a sphere
        SceneNodePtr nodeSphere = scene->GetRootNode().CreateChildNode("nodeSphere");
        ModelPtr sphere = mEngine.GetResourceManifest().CreateSphere("Sphere", 1.0f, 12, 24, checkerMaterial);
        Actor* actorSphere = scene->CreateActor("actorSphere", sphere, nodeSphere);
        nodeSphere->TranslateNode(Vec3(6.0f, 5.5f, 10.0f));

        // create a  second cube
        SceneNodePtr nodeCube2 = scene->GetRootNode().CreateChildNode("nodeCube2");
        ModelPtr cube2 = mEngine.GetResourceManifest().CreateCube("Cube2", 3, checkerMaterial);
        Actor* actorCube2 = scene->CreateActor("actorCube2", cube2, nodeCube2);
        nodeCube2->TranslateNode(Vec3(3.0f, 2.0f, -15.0f));

        // create a third cube, far away casting shadows from the dir light
        SceneNodePtr nodeCube3 = scene->GetRootNode().CreateChildNode("nodeCube3");
        ModelPtr cube3 = mEngine.GetResourceManifest().CreateCube("Cube3", 3, checkerMaterial);
        Actor* actorCube3 = scene->CreateActor("actorCube3", cube3, nodeCube3);
        nodeCube3->TranslateNode(-directionalLight->mLightDirection * 40.0f);

        // load skybox
        // TODO: textures UVs
        //const SkyboxPtr skybox = mEngine.GetResourceManifest().LoadSkybox("skybox", jonsPackage);
        //scene->SetSkybox(skybox);

        // move up camera
        scene->GetSceneCamera().TranslateCamera(Vec3(0.0f, 3.0f, 0.0f));
    }

    void Game::UpdateSun()
    {
        Scene* scene = mEngine.GetSceneManager().GetActiveScene();

        const uint64_t time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        const uint32_t angleDegreesTime = time % 360;
        const float angleRad = glm::radians(static_cast<float>(angleDegreesTime)) / 60.0f;

        DirectionalLight* directionalLight = scene->GetDirectionalLight("DirectionalLight");
        directionalLight->mLightDirection = glm::rotateY(directionalLight->mLightDirection, angleRad);
    }
}
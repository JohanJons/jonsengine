#include "include/Game.h"

#include "include/Sun.h"

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
    Game::Game() : mEngine(mSettings), mGameScene(*mEngine.GetSceneManager().GetActiveScene(), mEngine.GetResourceManifest(), "assets.jons"), mSun(mGameScene.GetSunDirLight()), mRunning(true), mMoveSpeed(0.1f)
    {
        SetupInputCallbacks();
        mEngine.GetWindow().ShowMouseCursor(false);
    }
        
    Game::~Game()
    {
    }

        
    void Game::Run()
    {
        while (mRunning)
        {
            mSun.Update();

            mEngine.Tick(mDebugOptions);
        }
    }

    void Game::OnKeyEvent(const KeyEvent& evnt)
    {
        Camera& camera = mGameScene.GetJonsScene().GetSceneCamera();

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
                case Key::Q: mSun.mIsMoving = true; break;
                case Key::W: mSun.mIsMoving = false; break;
                //case Key::E: mSunSpeed += 0.005f; break;
                //case Key::R: mSunSpeed -= 0.005f; break;
                
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
                case Key::Q: mGameScene.GetMovingPointLight().mSceneNode->TranslateNode(Vec3(-0.05f, 0.0f, 0.0f)); break;
                case Key::E: mGameScene.GetMovingPointLight().mSceneNode->TranslateNode(Vec3(0.05f, 0.0f, 0.0f));  break;
                case Key::R: mGameScene.GetMovingPointLight().mSceneNode->TranslateNode(Vec3(0.0f, -0.05f, 0.0f)); break;
                case Key::T: mGameScene.GetMovingPointLight().mSceneNode->TranslateNode(Vec3(0.0f, 0.05f, 0.0f));  break;
                case Key::F: mGameScene.GetMovingPointLight().mSceneNode->TranslateNode(Vec3(0.0f, 0.0f, -0.05f)); break;
                case Key::G: mGameScene.GetMovingPointLight().mSceneNode->TranslateNode(Vec3(0.0f, 0.0f, 0.05f));  break;

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
        Camera& camera = mGameScene.GetJonsScene().GetSceneCamera();

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

    //void Game::UpdateSun(const float initAngleSun)
    //{
    //    Scene* scene = mEngine.GetSceneManager().GetActiveScene();

    //    const uint64_t time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() % 360;
    //    const float angleDegreesTime = glm::radians(static_cast<float>(time));
    //    //const float angleRad = glm::radians(static_cast<float>(angleDegreesTime)) / 60.0f;

    //    const float alpha = glm::radians(-90.0f + static_cast<float>(angleDegreesTime));

    //    DirectionalLight* directionalLight = scene->GetDirectionalLight("DirectionalLight");
    //    directionalLight->mLightDirection = Vec3(glm::cos(angleDegreesTime), glm::sin(angleDegreesTime), 0.0f);//Vec3(glm::sin(initAngleSun) * glm::cos(alpha), glm::cos(initAngleSun), 0.0f);
    //    /*const float n = 5669.97f;
    //    const float eclipticObliquity = 23.439f - 0.0000004f * n;
    //    const float g = glm::radians(357.528f) + glm::radians(0.9856003f * n);
    //    const float R = 1.00014f - 0.01671f * glm::cos(g) - 0.00014f * glm::cos(2 * g);
    //    
    //    const float X = */
    //}
}
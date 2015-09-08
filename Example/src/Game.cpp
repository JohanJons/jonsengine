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
    Game::Game() : mEngine(mSettings), mGameScene(mEngine.GetSceneManager().GetActiveScene(), mEngine.GetResourceManifest(), "assets.jons"), mSun(mGameScene.GetSun()), mRunning(true), mMoveSpeed(0.1f)
    {
        SetupInputCallbacks();
        mEngine.GetWindow().ShowMouseCursor(false);
    }
        
    Game::~Game()
    {
    }

        
    void Game::Run()
    {
        IDMapTree<JonsEngine::Actor> tree;
        tree.AddNode("a", 1, 1);
        tree.AddNode("b", 2, 2);
        tree.AddNode("c", 3, 3);
        tree.FreeNode(1);

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
                case Key::Q: mSun.SetIsMoving(true); break;
                case Key::W: mSun.SetIsMoving(false); break;
                //case Key::E: mGameScene.GetAnimatedWuson().mModel->PlayAnimation("Wuson_Run");
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
                case Key::Q: mGameScene.GetMovingLightNode().TranslateNode(Vec3(-0.05f, 0.0f, 0.0f)); break;
                case Key::E: mGameScene.GetMovingLightNode().TranslateNode(Vec3(0.05f, 0.0f, 0.0f));  break;
                case Key::R: mGameScene.GetMovingLightNode().TranslateNode(Vec3(0.0f, -0.05f, 0.0f)); break;
                case Key::T: mGameScene.GetMovingLightNode().TranslateNode(Vec3(0.0f, 0.05f, 0.0f));  break;
                case Key::F: mGameScene.GetMovingLightNode().TranslateNode(Vec3(0.0f, 0.0f, -0.05f)); break;
                case Key::G: mGameScene.GetMovingLightNode().TranslateNode(Vec3(0.0f, 0.0f, 0.05f));  break;

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
}
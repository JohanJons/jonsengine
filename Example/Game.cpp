#include "Game.h"

#include "Sun.h"

#include "Core/Logging/Logger.h"
#include "Core/Types.h"
#include "Core/Containers/IDMap.hpp"
#include "Core/Utils/Timer.h"
#include "Scene/Scene.h"
#include "Resources/ResourceManifest.h"

#include "boost/bind.hpp"

using namespace std;
using namespace JonsEngine;

/*
 * Demo/interactive test of JonsEngine features
 * Requires JonsEngine binary asset file (.jons) to load art resources - see AssetImporter project
 */

namespace JonsGame
{
    Game::Game() : 
		mEngine(mSettings),
		mGameScene( mEngine.GetSceneManager(), mEngine.GetResourceManifest() ),
		mTessellationScene( mEngine.GetSceneManager(), mEngine.GetResourceManifest() )
    {
        SetupInputCallbacks();
        mEngine.GetWindow().ShowMouseCursor(false);

        mEngine.GetSceneManager().SetActiveScene( mTessellationScene.GetSceneID() );
        mIsDefaultScene = false;
    }
        
    Game::~Game()
    {
    }

        
    void Game::Run()
    {
        while (mRunning)
        {
			if ( mIsDefaultScene )
				mGameScene.Update();
			else
				mTessellationScene.Update();

            mEngine.Tick(mDebugOptions);
        }
    }

    void Game::OnKeyEvent(const KeyEvent& evnt)
    {
		Camera& camera = mEngine.GetSceneManager().GetActiveScene().GetSceneCamera();
		const bool isKeyPressed = evnt.mState == KeyEvent::KeyState::STATE_PRESSED || evnt.mState == KeyEvent::KeyState::STATE_REPEAT;
		if ( !isKeyPressed )
			return;

        // SHIFT-modifier activated
        if ( evnt.mShiftPressed )
        {
			switch ( evnt.mKey )
			{
				// camera
				case Key::A: camera.TranslateCamera( -camera.Right() * mMoveSpeed * 30.0f );   break;
				case Key::W: camera.TranslateCamera( camera.Forward() * mMoveSpeed * 30.0f );  break;
				case Key::S: camera.TranslateCamera( -camera.Forward() * mMoveSpeed * 30.0f ); break;
				case Key::D: camera.TranslateCamera( camera.Right() * mMoveSpeed * 30.0f );    break;
				default:
					break;
			}

			if ( mIsDefaultScene )
			{
				switch ( evnt.mKey )
				{
					//  renderering
					case Key::ONE: mEngine.GetRenderer().SetShadowFiltering( RenderSettings::ShadowFiltering::PCF_2x2 ); break;
					case Key::TWO: mEngine.GetRenderer().SetShadowFiltering( RenderSettings::ShadowFiltering::PCF_3x3 ); break;
					case Key::THREE: mEngine.GetRenderer().SetShadowFiltering( RenderSettings::ShadowFiltering::PCF_5x5 ); break;
					case Key::FOUR: mEngine.GetRenderer().SetShadowFiltering( RenderSettings::ShadowFiltering::PCF_7x7 ); break;

					case Key::FIVE: mEngine.GetRenderer().SetToneMappingAlghorithm( RenderSettings::ToneMappingAlghorithm::None ); break;
					case Key::SIX: mEngine.GetRenderer().SetToneMappingAlghorithm( RenderSettings::ToneMappingAlghorithm::FilmicU2 ); break;

					// animated actors
					case Key::E: mGameScene.ToggleAnimationWuson(); break;
					case Key::R: mGameScene.ToggleAnimationBob(); break;

					default:
						break;
				}
			}
			else
			{
				switch ( evnt.mKey )
				{
					case Key::ONE: mTessellationScene.FlipTerrain(); break;
				}
			}
        }
        // no modifiers
        else
        {
			switch ( evnt.mKey )
			{
				// camera
				case Key::A: camera.TranslateCamera( -camera.Right() * mMoveSpeed );   break;
				case Key::W: camera.TranslateCamera( camera.Forward() * mMoveSpeed );  break;
				case Key::S: camera.TranslateCamera( -camera.Forward() * mMoveSpeed ); break;
				case Key::D: camera.TranslateCamera( camera.Right() * mMoveSpeed );    break;

				//  misc
				case Key::N: mEngine.GetWindow().SetFullscreen( true ); break;
				case Key::M: mEngine.GetWindow().SetFullscreen( false ); break;
				case Key::V: mEngine.GetWindow().ShowMouseCursor( true ); break;
				case Key::B: mEngine.GetWindow().ShowMouseCursor( false ); break;
				case Key::X: mEngine.GetWindow().SetScreenResolution( 1080, 720 ); break;
				case Key::C: mEngine.GetWindow().SetScreenResolution( 1920, 1080 ); break;
				case Key::ESC: mRunning = false; break;

				// scene-switch
				case Key::F1: mEngine.GetSceneManager().SetActiveScene( mGameScene.GetSceneID() ); mIsDefaultScene = true; break;
				case Key::F2: mEngine.GetSceneManager().SetActiveScene( mTessellationScene.GetSceneID() ); mIsDefaultScene = false; break;

				default:
					break;
			}

			if ( mIsDefaultScene )
			{
				switch ( evnt.mKey )
				{
					// moving the point light 
					case Key::Q: mGameScene.GetMovingLightNode().TranslateNode( Vec3( -0.05f, 0.0f, 0.0f ) ); break;
					case Key::E: mGameScene.GetMovingLightNode().TranslateNode( Vec3( 0.05f, 0.0f, 0.0f ) );  break;
					case Key::R: mGameScene.GetMovingLightNode().TranslateNode( Vec3( 0.0f, -0.05f, 0.0f ) ); break;
					case Key::T: mGameScene.GetMovingLightNode().TranslateNode( Vec3( 0.0f, 0.05f, 0.0f ) );  break;
					case Key::F: mGameScene.GetMovingLightNode().TranslateNode( Vec3( 0.0f, 0.0f, -0.05f ) ); break;
					case Key::G: mGameScene.GetMovingLightNode().TranslateNode( Vec3( 0.0f, 0.0f, 0.05f ) );  break;

					//  renderering
					case Key::ONE: mDebugOptions.mRenderingFlags.flip( DebugOptions::RENDER_FLAG_DRAW_MODEL_AABB ); break;
					case Key::FOUR: mDebugOptions.mRenderingFlags.flip( DebugOptions::RENDER_FLAG_SHADOWMAP_SPLITS ); break;
					case Key::U: mEngine.GetRenderer().SetSSAO( false ); break;
					case Key::I: mEngine.GetRenderer().SetSSAO( true ); break;
					case Key::H: mEngine.GetRenderer().SetAnisotropicFiltering( RenderSettings::Anisotropic::X1 ); break;
					case Key::J: mEngine.GetRenderer().SetAnisotropicFiltering( RenderSettings::Anisotropic::X16 ); break;
					case Key::K: mEngine.GetRenderer().SetAntiAliasing( RenderSettings::AntiAliasing::None ); break;
					case Key::L: mEngine.GetRenderer().SetAntiAliasing( RenderSettings::AntiAliasing::Fxaa ); break;

					default:
						break;
				}
			}
			else
			{
				switch ( evnt.mKey )
				{
					case Key::ONE: mTessellationScene.GetTerrain().SetHeightScale( 80.0f ); break;
					case Key::TWO: mTessellationScene.GetTerrain().SetHeightScale( 120.0f ); break;
					case Key::THREE: mTessellationScene.GetTerrain().SetVariationScale( 3.0f ); break;
					case Key::FOUR: mTessellationScene.GetTerrain().SetVariationScale( 6.0f ); break;
					case Key::FIVE: mEngine.GetRenderer().SetTerrainPatchSize( RenderSettings::TerrainPatchSize::X16 ); break;
					case Key::SIX: mEngine.GetRenderer().SetTerrainPatchSize( RenderSettings::TerrainPatchSize::X32 ); break;
					case Key::SEVEN: mDebugOptions.mRenderingFlags.flip( DebugOptions::RENDER_FLAG_DRAW_TERRAIN_AABB ); break;
					case Key::EIGHT:
					{
						// TERRAIN_COPLANARITY / TERRAIN_WIREFRAME mutually exclusive debug options
						mDebugOptions.mRenderingFlags.set( DebugOptions::RENDER_FLAG_DRAW_TERRAIN_COPLANARITY, false ); 
						mDebugOptions.mRenderingFlags.flip( DebugOptions::RENDER_FLAG_DRAW_TERRAIN_WIREFRAME );
						break;
					}
					case Key::NINE:
					{
						mDebugOptions.mRenderingFlags.set( DebugOptions::RENDER_FLAG_DRAW_TERRAIN_WIREFRAME, false ); 
						mDebugOptions.mRenderingFlags.flip( DebugOptions::RENDER_FLAG_DRAW_TERRAIN_COPLANARITY );
						break;
					}
				}
			}
        }
    }

    void Game::OnMouseButtonEvent(const JonsEngine::MouseButtonEvent& evnt)
    {
    }

    void Game::OnMouseMovementEvent(const JonsEngine::MouseMovementEvent& evnt)
    {
        Camera& camera = mEngine.GetSceneManager().GetActiveScene().GetSceneCamera();

        const float sens = 0.1f;
        float newXPos = (float)evnt.x * sens;
        float newYPos = (float)evnt.y * sens;

        camera.RotateCamera(newXPos, newYPos);
    }

    void Game::SetupInputCallbacks()
    {
        mEngine.GetWindow().SetMouseButtonCallback(boost::bind(&Game::OnMouseButtonEvent, this, _1));
        mEngine.GetWindow().SetMouseMovementCallback(boost::bind(&Game::OnMouseMovementEvent, this, _1));
        mEngine.GetWindow().SetKeyCallback(boost::bind(&Game::OnKeyEvent, this, _1));
    }
}
#include "SceneTessellation.h"

#include "Core/Types.h"
#include "Scene/SceneManager.h"

using namespace JonsEngine;

constexpr float gTerrainHeightScale = 128.0f;
constexpr float gTerrainVariationScale = 2.0f;

namespace JonsGame
{
	SceneTessellation::SceneTessellation( JonsEngine::DX11Renderer& renderer, JonsEngine::SceneManager& sceneMgr, JonsEngine::ResourceManifest& resManifest ) :
		mID( sceneMgr.CreateScene( "TessellationScene" ) ),
		mRenderer( renderer ),
		mSceneMgr( sceneMgr ),
		mJonsPackage( ReadJonsPkg( "assets.jons" ) ),
		mSkybox( resManifest.LoadSkybox( "skybox", mJonsPackage ) ),
		mNodeTerrain( sceneMgr.GetScene( mID ).CreateSceneNode( "nodeTerrain", sceneMgr.GetScene( mID ).GetRootNodeID() ) ),
		mNodeTerrainTest( sceneMgr.GetScene( mID ).CreateSceneNode( "nodeTerrainTest", sceneMgr.GetScene( mID ).GetRootNodeID() ) ),
		mTerrainDataTest( resManifest.CreateTerrainData( "test", mJonsPackage ) ),
		mTerrainData( resManifest.CreateTerrainData( "mesopotamia", mJonsPackage ) ),
		mTerrainTest( sceneMgr.GetScene( mID ).CreateTerrain( "terrainTest", gTerrainHeightScale, gTerrainVariationScale, mNodeTerrainTest, mTerrainDataTest ) ),
		mTerrain( sceneMgr.GetScene( mID ).CreateTerrain( "terrain", gTerrainHeightScale, gTerrainVariationScale, mNodeTerrain, mTerrainData ) ),
		mSunDirLightID( sceneMgr.GetScene( mID ).CreateDirectionalLight( "DirectionalLight", 4 ) ),
		mSun( mSceneMgr.GetScene( mID ).GetDirectionalLight( mSunDirLightID ) )
	{
		JonsEngine::Scene& scene = sceneMgr.GetScene( mID );

		scene.SetAmbientLight( Vec4( 0.2f ) );

		DirectionalLight& sun = scene.GetDirectionalLight( mSunDirLightID );
		sun.SetLightColor( Vec4( 6.0f ) );
		sun.SetLightDirection( Vec3( -0.735238075f, -0.607724974f, -0.645194054f ) );

		SceneNode& nodeTerrain = scene.GetSceneNode( mNodeTerrain );
		SceneNode& nodeTerrainTest = scene.GetSceneNode( mNodeTerrainTest );
		nodeTerrain.TranslateNode( Vec3( 0.0f, 0.0f, 0.0f ) );
		nodeTerrainTest.TranslateNode( Vec3( 300.0f, 670.0f, 0.0f ) );
		nodeTerrainTest.SetVisible( false );

		Terrain& terrainMeso = scene.GetTerrain( mTerrain );
		const TerrainData& terrainDataMeso = resManifest.GetTerrainData( terrainMeso.GetTerrainData() );

		float moistureFalloffBegin = std::log2( terrainDataMeso.GetWidth() );
		float moistureFalloffDistance = moistureFalloffBegin * 20.0f;

		terrainMeso.SetMoistureFalloffBegin( moistureFalloffBegin );
		terrainMeso.SetMoistureFalloffDistance( moistureFalloffDistance );

		scene.SetSkybox( mSkybox );

		scene.GetSceneCamera().SetPosition( { 1058.0f, 140.0f, -235.0f } );

		mRenderer.SetZNear( 2.0f );
		mRenderer.SetZFar( 4098.0f );

		//FlipTerrain();
	}

	SceneTessellation::~SceneTessellation()
	{
		// TODO: cleanup...
	}


	void SceneTessellation::Update()
	{
		//mSun.Update();
	}

	void SceneTessellation::FlipTerrain()
	{
		JonsEngine::Scene& scene = mSceneMgr.GetScene( mID );

		SceneNode& nodeTerrain = scene.GetSceneNode( mNodeTerrain );
		SceneNode& nodeTerrainTest = scene.GetSceneNode( mNodeTerrainTest );

		nodeTerrain.SetVisible( !nodeTerrain.IsVisible() );
		nodeTerrainTest.SetVisible( !nodeTerrainTest.IsVisible() );
		
		if ( nodeTerrainTest.IsVisible() )
		{
			mRenderer.SetZNear( 1.0f );
			mRenderer.SetZFar( 1024.0f );
			scene.GetSceneCamera().SetPosition( { 211.4f, 702.1f, 32.9f } );
		}
		else
		{
			mRenderer.SetZNear( 2.0f );
			mRenderer.SetZFar( 4098.0f );
			scene.GetSceneCamera().SetPosition( { 1058.0f, 40.0f, -235.0f } );
		}
	}

	JonsEngine::Scene& SceneTessellation::GetJonsScene()
	{
		return mSceneMgr.GetScene( mID );
	}

	JonsEngine::Terrain& SceneTessellation::GetTerrain()
	{
		return mSceneMgr.GetScene( mID ).GetTerrain( mTerrain );
	}
}
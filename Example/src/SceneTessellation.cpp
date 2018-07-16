#include "include/SceneTessellation.h"

#include "include/Core/Types.h"
#include "include/Scene/SceneManager.h"

using namespace JonsEngine;

constexpr float gTerrainHeightScale = 64.0f;
constexpr uint32_t gTerrainPatchSize = 64;

namespace JonsGame
{
	SceneTessellation::SceneTessellation( JonsEngine::SceneManager& sceneMgr, JonsEngine::ResourceManifest& resManifest ) :
		// base
		mID( sceneMgr.CreateScene( "TessellationScene" ) ),
		mSceneMgr( sceneMgr ),
		mJonsPackage( ReadJonsPkg( "assets.jons" ) ),

		mSkybox( resManifest.LoadSkybox( "skybox", mJonsPackage ) ),

		mNodeTerrain( sceneMgr.GetScene( mID ).CreateSceneNode( "nodeTerrain", sceneMgr.GetScene( mID ).GetRootNodeID() ) ),
		mTerrainData( resManifest.CreateTerrainData( "terraindata", "heightmap", mJonsPackage ) ),
		mTerrain( sceneMgr.GetScene( mID ).CreateTerrain( "terrain", gTerrainHeightScale, gTerrainPatchSize, mNodeTerrain, mTerrainData ) ),

		mSunDirLightID( sceneMgr.GetScene( mID ).CreateDirectionalLight( "DirectionalLight", 4 ) )
	{
		JonsEngine::Scene& scene = sceneMgr.GetScene( mID );

		scene.SetAmbientLight( Vec4( 0.03f ) );

		DirectionalLight& sun = scene.GetDirectionalLight( mSunDirLightID );
		sun.SetLightColor( Vec4( 6.0f ) );

		SceneNode& nodeTerrain = scene.GetSceneNode( mNodeTerrain );
		nodeTerrain.TranslateNode( Vec3( 0.0f, 0.0f, 0.0f ) );

		scene.SetSkybox( mSkybox );

		scene.GetSceneCamera().TranslateCamera( Vec3( 0.0f, 6.0f, 0.0f ) );
	}

	SceneTessellation::~SceneTessellation()
	{
		// TODO: cleanup...
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
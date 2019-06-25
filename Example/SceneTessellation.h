#pragma once

#include "Core/Engine.h"
#include "Resources/JonsPackage.h"
#include "Scene/Scene.h"

#include "Sun.h"

#include <string>

namespace JonsGame
{
	class SceneTessellation
	{
	public:
		SceneTessellation( JonsEngine::SceneManager& sceneMgr, JonsEngine::ResourceManifest& resManifest );
		~SceneTessellation();

		void Update();
		void FlipTerrain();

		JonsEngine::SceneID GetSceneID() const { return mID; }
		JonsEngine::SceneID GetTestSceneID() const { return mID; }

		JonsEngine::Scene& GetJonsScene();
		JonsEngine::Terrain& GetTerrain();

	private:
		const JonsEngine::SceneNodeID mID;
		JonsEngine::SceneManager& mSceneMgr;
		JonsEngine::JonsPackagePtr mJonsPackage;

		JonsEngine::SkyboxID mSkybox;

		JonsEngine::SceneNodeID mNodeTerrain;
		JonsEngine::SceneNodeID mNodeTerrainTest;
		JonsEngine::TerrainDataID mTerrainData;
		JonsEngine::TerrainDataID mTerrainDataTest;
		JonsEngine::TerrainID mTerrain;
		JonsEngine::TerrainID mTerrainTest;

		JonsEngine::DirectionalLightID mSunDirLightID;
		Sun mSun;
	};
}
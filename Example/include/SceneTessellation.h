#pragma once

#include "include/Core/Engine.h"
#include "include/Resources/JonsPackage.h"
#include "include/Scene/Scene.h"
#include "include/Sun.h"

#include <string>

namespace JonsGame
{
	class SceneTessellation
	{
	public:
		SceneTessellation( JonsEngine::SceneManager& sceneMgr, JonsEngine::ResourceManifest& resManifest );
		~SceneTessellation();

		void Update();

		JonsEngine::SceneID GetSceneID() const { return mID; }

		JonsEngine::Scene& GetJonsScene();
		JonsEngine::Terrain& GetTerrain();

	private:
		const JonsEngine::SceneNodeID mID;
		JonsEngine::SceneManager& mSceneMgr;
		JonsEngine::JonsPackagePtr mJonsPackage;

		JonsEngine::SkyboxID mSkybox;

		JonsEngine::SceneNodeID mNodeTerrain;
		JonsEngine::TerrainDataID mTerrainData;
		JonsEngine::TerrainID mTerrain;

		JonsEngine::DirectionalLightID mSunDirLightID;
		Sun mSun;
	};
}
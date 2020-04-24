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
		SceneTessellation( JonsEngine::DX11Renderer& renderer, JonsEngine::SceneManager& sceneMgr, JonsEngine::ResourceManifest& resManifest );
		~SceneTessellation();

		void Update();
		void FlipTerrain();

		JonsEngine::SceneID GetSceneID() const { return mID; }
		JonsEngine::SceneID GetTestSceneID() const { return mID; }

		JonsEngine::Scene& GetJonsScene();
		JonsEngine::Terrain& GetTerrain();

	private:
		const JonsEngine::SceneNodeID mID;
		JonsEngine::DX11Renderer& mRenderer;
		JonsEngine::SceneManager& mSceneMgr;
		JonsEngine::JonsPackagePtr mJonsPackage;

		JonsEngine::SkyboxID mSkybox;

		JonsEngine::SceneNodeID mNodeTerrain;
		JonsEngine::SceneNodeID mNodeTerrainTest;
		JonsEngine::TerrainDataID mTerrainDataTest;
		JonsEngine::TerrainDataID mTerrainData;
		JonsEngine::TerrainID mTerrainTest;
		JonsEngine::TerrainID mTerrain;

		JonsEngine::DirectionalLightID mSunDirLightID;
		Sun mSun;
	};
}
#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Resources/Model.h"
#include "include/Resources/TerrainData.h"

#include <string>

namespace JonsEngine
{
	class Scene;
	class Terrain;
	typedef std::function<void( Terrain* )> OnTerrainDirtyFunc;

	class Terrain
	{
	public:
		Terrain(const std::string& name, float heightScale, uint32_t patchSize, float heightmapMultiplyer, const SceneNodeID sceneNodeID, const TerrainDataID terrainDataID, const OnTerrainDirtyFunc& onDirtyFunc );

		void SetHeightScale( float heightScale );
		float GetHeightScale() const { return mHeightScale; }

		void SetPatchSize(uint32_t patchSize);
		uint32_t GetPatchSize() const { return mPatchSize; }

		// terrain size in relation to the heightmap
		void SetHeightmapMultiplyer( float heightmapMultiplyer );
		float GetHeightmapMultiplyer() const { return mHeightmapMultiplyer; }

		const std::string& GetName() const { return mName; }
		SceneNodeID GetSceneNode() const { return mSceneNodeID; }
		TerrainDataID GetTerrainData() const { return mTerrainDataID; }

	private:
		friend class Scene;
		void SetCallback( const OnTerrainDirtyFunc& onDirty );

	private:
		std::string mName;
		float mHeightScale;
		float mHeightmapMultiplyer;
		uint32_t mPatchSize;

		SceneNodeID mSceneNodeID = INVALID_SCENE_NODE_ID;
		TerrainDataID mTerrainDataID = INVALID_TERRAIN_DATA_ID;
		OnTerrainDirtyFunc mOnDirtyFunc;
	};

	typedef IDMap<Terrain>::ItemID TerrainID;
	static const TerrainID INVALID_TERRAIN_ID = IDMap<TerrainID>::INVALID_ITEM_ID;
}
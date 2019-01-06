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
		Terrain(const std::string& name, float heightScale, float variationScale, uint32_t patchSize, float terrainSizeMultiplyer, const SceneNodeID sceneNodeID, const TerrainDataID terrainDataID, const OnTerrainDirtyFunc& onDirtyFunc );

		void SetHeightScale( float heightScale );
		float GetHeightScale() const { return mHeightScale; }

		// random variation applied along the normal
		void SetVariationScale( float variationScale );
		float GetVariationScale() const { return mVariationScale; }

		void SetPatchSize(uint32_t patchSize);
		uint32_t GetPatchSize() const { return mPatchSize; }

		// terrain size in relation to the heightmap
		void SetTerrainSizeMultiplyer( float terrainSizeMultiplyer );
		float GetTerrainSizeMultiplyer() const { return mTerrainSizeMultiplyer; }

		const std::string& GetName() const { return mName; }
		SceneNodeID GetSceneNode() const { return mSceneNodeID; }
		TerrainDataID GetTerrainData() const { return mTerrainDataID; }

	private:
		friend class Scene;
		void SetCallback( const OnTerrainDirtyFunc& onDirty );

	private:
		std::string mName;
		float mHeightScale;
		float mVariationScale;
		float mTerrainSizeMultiplyer;
		uint32_t mPatchSize;

		SceneNodeID mSceneNodeID = INVALID_SCENE_NODE_ID;
		TerrainDataID mTerrainDataID = INVALID_TERRAIN_DATA_ID;
		OnTerrainDirtyFunc mOnDirtyFunc;
	};

	typedef IDMap<Terrain>::ItemID TerrainID;
	static const TerrainID INVALID_TERRAIN_ID = IDMap<TerrainID>::INVALID_ITEM_ID;
}
#pragma once

#include "Scene/SceneNode.h"
#include "Resources/Model.h"
#include "Resources/TerrainData.h"

#include <string>

namespace JonsEngine
{
	class Scene;
	class Terrain;
	typedef std::function<void( Terrain* )> OnTerrainDirtyFunc;

	class Terrain
	{
	public:
		Terrain(const std::string& name, float heightScale, float variationScale, const SceneNodeID sceneNodeID, const TerrainDataID terrainDataID, const OnTerrainDirtyFunc& onDirtyFunc );

		void SetHeightScale( float heightScale );
		float GetHeightScale() const { return mHeightScale; }

		// random variation applied along the normal
		void SetVariationScale( float variationScale );
		float GetVariationScale() const { return mVariationScale; }

		void SetMoistureFalloffBegin( float moistureFalloffBegin ) { mMoistureFalloffBegin = moistureFalloffBegin; }
		float GetMoistureFalloffBegin() const { return mMoistureFalloffBegin; }

		void SetMoistureFalloffDistance( float moistureFalloffDistance ) { mMoistureFalloffDistance = moistureFalloffDistance; }
		float GetMoistureFalloffDistance() const { return mMoistureFalloffDistance; }

		const std::string& GetName() const { return mName; }
		SceneNodeID GetSceneNode() const { return mSceneNodeID; }
		TerrainDataID GetTerrainData() const { return mTerrainDataID; }

	private:
		friend class Scene;
		void SetCallback( const OnTerrainDirtyFunc& onDirty );

	private:
		std::string mName;
		float mHeightScale = 0.0f;
		float mVariationScale = 0.0f;
		float mMoistureFalloffBegin = 0.0f;
		float mMoistureFalloffDistance = 0.0f;

		SceneNodeID mSceneNodeID = INVALID_SCENE_NODE_ID;
		TerrainDataID mTerrainDataID = INVALID_TERRAIN_DATA_ID;
		OnTerrainDirtyFunc mOnDirtyFunc;
	};

	typedef IDMap<Terrain>::ItemID TerrainID;
	static const TerrainID INVALID_TERRAIN_ID = IDMap<TerrainID>::INVALID_ITEM_ID;
}
#pragma once

#include "include/Core/Types.h"
#include "include/Core/Math/AABB.h"
#include "include/Core/Containers/IDMap.hpp"

#include <string>

namespace JonsEngine
{
	class TerrainData
	{
	public:
		TerrainData(const std::string& name, float minElevation, float maxElevation, float width, float height, DX11TextureID heightMap);

		const std::string& GetName() const { return mName; }
		const AABB& GetStaticAABB() const { return mStaticAABB; }
		float GetWidth() const { return mWidth; }
		float GetHeight() const { return mHeight; }
		DX11TextureID GetHeightMap() const { return mHeightMap; }


	private:
		std::string mName;
		AABB mStaticAABB;
		float mWidth;
		float mHeight;
		DX11TextureID mHeightMap;
	};

	typedef IDMap<TerrainData>::ItemID TerrainDataID;
	static const TerrainDataID INVALID_TERRAIN_DATA_ID = IDMap<TerrainDataID>::INVALID_ITEM_ID;
}
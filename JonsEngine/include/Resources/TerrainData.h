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
		TerrainData(const std::string& name, float minElevation, float maxElevation, uint32_t width, uint32_t height, DX11TextureID heightMap, const std::vector<uint8_t>& heightmapData);

		const std::string& GetName() const { return mName; }
		const AABB& GetStaticAABB() const { return mStaticAABB; }
		uint32_t GetWidth() const { return mWidth; }
		uint32_t GetHeight() const { return mHeight; }
		DX11TextureID GetHeightMap() const { return mHeightMap; }
		const std::vector<uint8_t>& GetHeightMapData() const { return mHeightMapData; }

	private:
		std::string mName;
		AABB mStaticAABB;
		uint32_t mWidth;
		uint32_t mHeight;
		DX11TextureID mHeightMap;
		std::vector<uint8_t> mHeightMapData;
	};

	typedef IDMap<TerrainData>::ItemID TerrainDataID;
	static const TerrainDataID INVALID_TERRAIN_DATA_ID = IDMap<TerrainDataID>::INVALID_ITEM_ID;
}
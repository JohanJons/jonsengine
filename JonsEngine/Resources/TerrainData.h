#pragma once

#include "Core/Types.h"
#include "Core/Math/AABB.h"
#include "Core/Containers/IDMap.hpp"

#include <string>

namespace JonsEngine
{
	class TerrainData
	{
	public:
		TerrainData(const std::string& name, uint32_t width, uint32_t height, DX11TextureID heightMap, TextureType heightmapType, const std::vector<uint8_t>& heightmapData );

		const std::string& GetName() const { return mName; }
		uint32_t GetWidth() const { return mWidth; }
		uint32_t GetHeight() const { return mHeight; }
		DX11TextureID GetHeightMap() const { return mHeightMap; }
		TextureType GetTextureType() const { return mTextureType; }
		const std::vector<uint8_t>& GetHeightMapData() const { return mHeightMapData; }

	private:
		std::string mName;
		uint32_t mWidth;
		uint32_t mHeight;
		DX11TextureID mHeightMap;
		TextureType mTextureType;
		std::vector<uint8_t> mHeightMapData;
	};

	typedef IDMap<TerrainData>::ItemID TerrainDataID;
	static const TerrainDataID INVALID_TERRAIN_DATA_ID = IDMap<TerrainDataID>::INVALID_ITEM_ID;
}
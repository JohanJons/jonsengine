#pragma once

#include "include/Core/Types.h"
#include "include/Core/Containers/IDMap.hpp"

#include <string>

namespace JonsEngine
{
	class Terrain
	{
	public:
		Terrain(const std::string& name, const DX11MeshID groundMesh, const DX11TextureID heightMap);

		const std::string& GetName() const;
		DX11MeshID GetMesh() const;
		DX11TextureID GetHeightMap() const;


	private:
		std::string mName;

		DX11MeshID mGroundMesh;
		DX11TextureID mHeightMap;
	};

	typedef IDMap<Terrain>::ItemID TerrainID;
	static const TerrainID INVALID_TERRAIN_ID = IDMap<TerrainID>::INVALID_ITEM_ID;
}
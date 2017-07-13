#include "include/Resources/TerrainData.h"

namespace JonsEngine
{
	TerrainData::TerrainData(const std::string& name, float minElevation, float maxElevation, float width, float height, DX11MeshID groundMesh, DX11TextureID heightMap) :
		mName(name),
		mStaticAABB({ -(width / 2.0f), minElevation, -(height / 2.0f) }, { width / 2.0f, maxElevation, height / 2.0f }),
		mGroundMesh(groundMesh),
		mHeightMap(heightMap)
	{
	}


	const std::string& TerrainData::GetName() const
	{
		return mName;
	}

	const AABB& TerrainData::GetStaticAABB() const
	{
		return mStaticAABB;
	}

	DX11MeshID TerrainData::GetMesh() const
	{
		return mGroundMesh;
	}

	DX11TextureID TerrainData::GetHeightMap() const
	{
		return mHeightMap;
	}
}
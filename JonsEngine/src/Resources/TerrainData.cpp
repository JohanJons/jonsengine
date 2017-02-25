#include "include/Resources/TerrainData.h"

namespace JonsEngine
{
	TerrainData::TerrainData(const std::string& name, const DX11MeshID groundMesh, const DX11TextureID heightMap) :
		mName(name),
		mGroundMesh(groundMesh),
		mHeightMap(heightMap)
	{
	}


	const std::string& TerrainData::GetName() const
	{
		return mName;
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
#include "include/Scene/Terrain.h"

namespace JonsEngine
{
	Terrain::Terrain(const std::string& name, const DX11MeshID groundMesh, const DX11TextureID heightMap) :
		mName(name),
		mGroundMesh(groundMesh),
		mHeightMap(heightMap)
	{
	}


	const std::string& Terrain::GetName() const
	{
		return mName;
	}

	DX11MeshID Terrain::GetMesh() const
	{
		return mGroundMesh;
	}

	DX11TextureID Terrain::GetHeightMap() const
	{
		return mHeightMap;
	}
}
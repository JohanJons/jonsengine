#include "include/Resources/TerrainData.h"

#include "include/Renderer/DirectX11/DX11TerrainPass.h"

namespace JonsEngine
{
	TerrainData::TerrainData(const std::string& name, float minElevation, float maxElevation, float width, float height, DX11TextureID heightMap) :
		mName(name),
		mStaticAABB({ -(width / 2.0f), minElevation, -(height / 2.0f) }, { width / 2.0f, maxElevation, height / 2.0f }),
		mHeightMap(heightMap),
		mPatchSize(width / DX11TerrainPass::GRID_SIZE)
	{
		assert(maxElevation >= minElevation);
		// for now, only allow square heightmaps
		assert(width == height);
	}


	const std::string& TerrainData::GetName() const
	{
		return mName;
	}

	const AABB& TerrainData::GetStaticAABB() const
	{
		return mStaticAABB;
	}

	DX11TextureID TerrainData::GetHeightMap() const
	{
		return mHeightMap;
	}

	float TerrainData::GetPatchSize() const
	{
		return mPatchSize;
	}
}
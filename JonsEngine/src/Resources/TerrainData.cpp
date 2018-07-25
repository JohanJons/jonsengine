#include "include/Resources/TerrainData.h"

#include "include/Renderer/DirectX11/DX11TerrainPass.h"

namespace JonsEngine
{
	TerrainData::TerrainData( const std::string& name, float minElevation, float maxElevation, uint32_t width, uint32_t height, DX11TextureID heightMap, const std::vector<uint8_t>& heightmapData ) :
		mName( name ),
		mStaticAABB({ -(width / 2.0f), minElevation, -(height / 2.0f) }, { width / 2.0f, maxElevation, height / 2.0f }),
		mWidth( width ),
		mHeight( height ),
		mHeightMap( heightMap ),
		mHeightMapData( heightmapData )
	{
		assert(maxElevation >= minElevation);
		// for now, only allow square heightmaps
		assert(width == height);
	}
}
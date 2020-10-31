#include "Resources/TerrainData.h"

#include "Renderer/DirectX11/DX11TerrainPass.h"

namespace JonsEngine
{
	TerrainData::TerrainData( const std::string& name, uint32_t width, uint32_t height, DX11TextureID heightMap, DX11TextureID riversMap, TextureType heightmapType, const std::vector<uint8_t>& heightmapData ) :
		mName( name ),
		mWidth( width ),
		mHeight( height ),
		mHeightMap( heightMap ),
		mRiversMap( riversMap ),
		mTextureType( heightmapType ),
		mHeightMapData( heightmapData )
	{ }
}
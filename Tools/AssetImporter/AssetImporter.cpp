#include "AssetImporter.h"

#include "Utils.h"
#include "Assimp.h"
#include "FreeImageParser.h"
#include "TOMLParser.h"

using namespace JonsEngine;

namespace JonsAssetImporter
{
	TextureType GetTextureTypeFromString( const std::string& name )
	{
		if ( name.compare( "diffuse" ) == 0 )
			return TextureType::Diffuse;
		else if ( name.compare( "normal" ) == 0 )
			return TextureType::Normal;
		else if ( name.compare( "height8" ) == 0 )
			return TextureType::Height8;
		else if ( name.compare( "height16" ) == 0 )
			return TextureType::Height16;
		else if ( name.compare( "skybox" ) == 0 )
			return TextureType::Skybox;

		return TextureType::Unknown;
	}

    enum class ParamType
    {
        Unknown,
        Model,
		Texture_Diffuse,
		Texture_Normal,
		Texture_Height8,
		Texture_Height16,
        Skybox,
        AssetName,
		Package
    };

	void CreatePackageHeader( JonsPackagePtr jonsPkg, const TomlData& tomlPkg );
	void ImportModels( JonsPackagePtr jonsPkg, const TomlData& tomlPkg, Assimp& assimpImporter, FreeImage& freeimageParser );
	void ImportTextures( JonsPackagePtr jonsPkg, const TomlData& tomlPkg, FreeImage& freeimageParser );
	void ImportTerrains( JonsPackagePtr jonsPkg, const TomlData& tomlPkg );

	template <typename T>
	bool VerifyNoDuplicateName( const std::vector<T>& arr, const std::string& name );


    ParseResult ParseCommands( const std::vector<std::string>& parameters )
    {
        Assimp assimpImporter;
        FreeImage freeimageParser;

        if ( parameters.size() != 1 )
        {
            Log("ERROR: expected only 1 argument");
            return { false, FlushLog() };
        }

		const std::string& tomlFile = parameters.front();

		TomlData tomlPkg = LoadTOML( tomlFile );
		if ( tomlPkg.is_uninitialized() )
		{
			return { false, FlushLog() };
		}

		JonsPackagePtr jonsPkg( new JonsPackage() );

		CreatePackageHeader( jonsPkg, tomlPkg );
		ImportModels( jonsPkg, tomlPkg, assimpImporter, freeimageParser );
		ImportTextures( jonsPkg, tomlPkg, freeimageParser );
		ImportTerrains( jonsPkg, tomlPkg );

		std::string outfileName = GetTOMLData<std::string>( tomlPkg, "outfile" );
		WriteJonsPkg( outfileName, jonsPkg );

		return { true, FlushLog() };
    }

	void CreatePackageHeader( JonsPackagePtr jonsPkg, const TomlData& tomlPkg )
	{
		const TomlData& packageInfo = GetTOMLData<TomlData>( tomlPkg, "package" );

		jonsPkg->mHeader.mName = GetTOMLData<std::string>( packageInfo, "name" );
	}

	void ImportModels( JonsPackagePtr jonsPkg, const TomlData& tomlPkg, Assimp& assimpImporter, FreeImage& freeimageParser )
	{
		const auto& models = GetTOMLData<std::vector<toml::table>>( tomlPkg, "models" );
		if ( models.empty() )
			return;

		for ( const auto& modelEntry : models )
		{
			std::string name = GetTOMLData<std::string>( modelEntry, "name" );
			std::string path = GetTOMLData<std::string>( modelEntry, "path" );

			if ( !VerifyNoDuplicateName( jonsPkg->mModels, name ) )
			{
				Log( "Duplicate model name found, skipping: " + name );
				continue;
			}

			assimpImporter.ProcessScene( path, name, freeimageParser, jonsPkg );
		}
	}

	void ImportTextures( JonsPackagePtr jonsPkg, const TomlData& tomlPkg, FreeImage& freeimageParser )
	{
		const auto& textures = GetTOMLData<std::vector<toml::table>>( tomlPkg, "textures" );
		if ( textures.empty() )
			return;

		for ( const auto& textureEntry : textures )
		{
			std::string name = GetTOMLData<std::string>( textureEntry, "name" );
			std::string path = GetTOMLData<std::string>( textureEntry, "path" );
			TextureType type = GetTextureTypeFromString( GetTOMLData<std::string>( textureEntry, "type" ) );

			if ( !VerifyNoDuplicateName( jonsPkg->mTextures, name ) )
			{
				Log( "Duplicate texture name found, skipping: " + name );
				continue;
			}

			if ( type == TextureType::Skybox )
				freeimageParser.ProcessSkybox( path, name, jonsPkg);
			else
				freeimageParser.ProcessTexture2D( path, name, type, jonsPkg );
		}
	}

	void ImportTerrains( JonsPackagePtr jonsPkg, const TomlData& tomlPkg )
	{
		const auto& terrains = GetTOMLData<std::vector<toml::table>>( tomlPkg, "terrains" );
		if ( terrains.empty() )
			return;

		for ( const auto& terrainEntry : terrains )
		{
			std::string name = GetTOMLData<std::string>( terrainEntry, "name" );
			std::string heightmap = GetTOMLData<std::string>( terrainEntry, "heightmap" );

			std::string riversmap;
			if ( TOMLContains( terrainEntry, "rivermap" ) )
			{
				riversmap = GetTOMLData<std::string>( terrainEntry, "rivermap" );
			}

			jonsPkg->mTerrains.emplace_back( std::move( name ) );
			PackageTerrain& pkgTerrain = jonsPkg->mTerrains.back();

			if ( !heightmap.empty() )
			{
				pkgTerrain.mHeightmap = jonsPkg->FindTexture( heightmap );
			}

			if ( !riversmap.empty() )
			{
				pkgTerrain.mRivermap = jonsPkg->FindTexture( riversmap );
			}
		}
	}

	template <typename T>
	bool VerifyNoDuplicateName( const std::vector<T>& arr, const std::string& name )
	{
		auto iter = std::find_if( arr.begin(), arr.end(), [ &name ]( const T& entry ) { return entry.mName == name; } );
		return iter == arr.end();
	}
}
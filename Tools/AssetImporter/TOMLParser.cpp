#include "TOMLParser.h"

namespace JonsAssetImporter
{
	TomlData LoadTOML( const std::string& filePath )
	{
		TomlData data;
		try {
			data = toml::parse( filePath );
		}
		catch ( const std::runtime_error& e )
		{
			Log( e );
		}

		return data;
	}
}
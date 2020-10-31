#include "TOMLParser.h"

namespace JonsAssetImporter
{
	bool TOMLContains( const TomlData& package, const std::string& key )
	{
		try {
			const toml::value& value = toml::find( package, key );
			return !value.is_uninitialized();
		}
		catch ( const std::exception& )
		{
			return false;
		}
	}

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
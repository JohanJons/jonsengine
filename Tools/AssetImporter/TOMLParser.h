#pragma once

#include "toml.hpp"
#include "Utils.h"

namespace JonsAssetImporter
{
	using TomlData = toml::value;

	TomlData LoadTOML( const std::string& filePath );
	bool TOMLContains( const TomlData& package, const std::string& key );

	template <typename T>
	T GetTOMLData( const TomlData& package, const std::string& key )
	{
		T data;
		try {
			data = toml::find<T>( package, key );
		}
		catch ( const std::exception& e )
		{
			Log( e );
		}

		return data;
	}
}
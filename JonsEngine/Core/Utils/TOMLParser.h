#pragma once

#include "toml.hpp"

#include <functional>

namespace JonsEngine
{
	using TomlData = toml::value;

	TomlData LoadTOML( const std::string& filePath, std::function<void(const std::string&)> logFunc );
	bool TOMLContains( const TomlData& package, const std::string& key );

	template <typename T>
	T GetTOMLData( const TomlData& package, const std::string& key, std::function<void(const std::string&)> logFunc  )
	{
		T data;
		try {
			data = toml::find<T>( package, key );
		}
		catch ( const std::exception& e )
		{
			logFunc( e.what() );
		}

		return data;
	}
}
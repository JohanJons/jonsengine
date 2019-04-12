#pragma once

#include <bitset>

namespace JonsEngine
{
	enum DirtyFlags : std::size_t
	{
		FlagTerrain = 0,
		FlagCount
	};

	typedef std::bitset<DirtyFlags::FlagCount> DirtyFlagsSet;
}
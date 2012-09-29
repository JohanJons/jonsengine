#pragma once

// Use stdint.h defintion of int32_t, uint64_t, etc by default across the engine
#include <stdint.h>
#include <string>

namespace JonsEngine
{
	/*
	 * Logging
	 */

	// defined in LogManager.cpp
	extern const std::string GameEngineTag;
}

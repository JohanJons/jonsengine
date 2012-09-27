#pragma once

// Use stdint.h defintion of int32_t, uint64_t, etc by default across the engine
#include <stdint.h>
#include <string>

namespace JonsEngine
{
	/*
	 * Logging
	 */

	// Log tags; defined in LogManager.cpp
	extern const std::string GameEngineTag;
	extern const std::string EngineTag;
	extern const std::string ThreadingTag;
}

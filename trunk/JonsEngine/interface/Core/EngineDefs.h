#pragma once

// Use stdint.h defintion of int32_t, uint64_t, etc by default across the engine
#include <stdint.h>
#include <string>

namespace JonsEngine
{
	/*
	 * Logging
	 */

	// GameEngine Log tag
	const std::string GameEngineTag = "JonsEngine";

	// Engine module tag
	const std::string EngineTag = "ENGINE";

	// Threading module tag
	const std::string ThreadingTag = "THREADING";
}

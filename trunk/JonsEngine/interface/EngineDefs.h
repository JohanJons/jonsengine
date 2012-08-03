#ifndef _ENGINE_DEFS_H
#define _ENGINE_DEFS_H

// Use stdint.h defintion of int32_t, uint64_t, etc by default across the engine
#include <stdint.h>
#include <string>

namespace JonsEngine
{
	//
	// Logging
	//

	// Log tag
	const std::string EngineLogTag = "JonsEngine";

}

#endif

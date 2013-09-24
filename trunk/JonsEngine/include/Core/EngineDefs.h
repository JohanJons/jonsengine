#pragma once

// Use stdint.h defintion of int32_t, uint64_t, etc by default across the engine
#include <cstdint>
#include <cstddef>
#include <string>

namespace JonsEngine
{
    /*
     * Logging
     */

    #ifdef _DEBUG
        const bool is_debug = true;
    #else
        const bool is_debug = false;
    #endif

    const std::string GameEngineTag = "JonsEngine";
}

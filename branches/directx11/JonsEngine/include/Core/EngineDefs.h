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

    const std::string GameEngineTag = "JonsEngine";


	/*
	 * Win32 messages
	 */

	const uint32_t WM_JONS_FULLSCREEN = (WM_USER + 0x0001);
	const uint32_t WM_JONS_RESIZE = (WM_USER + 0x0002);
}

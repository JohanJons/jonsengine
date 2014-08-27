#pragma once

#include "include/Core/Types.h"

#include <Windows.h>


namespace JonsEngine
{

	/*
	 * Win32 messages
	 */

	const uint32_t WM_JONS_FULLSCREEN = (WM_USER + 0x0001);
	const uint32_t WM_JONS_RESIZE = (WM_USER + 0x0002);
}
#pragma once

#include "include/Core/Types.h"

#include <string>

namespace JonsEngine
{
	struct WindowSettings
	{
		std::string mWindowTitle;
		uint16_t mWindowWidth;
		uint16_t mWindowHeight;
		bool mFullscreen;
		uint16_t mFrameLimit;


		WindowSettings();
	};

	inline WindowSettings::WindowSettings() :
		mWindowTitle("JonsEngine Game"),
		mWindowWidth(1920),
		mWindowHeight(1080),
		mFullscreen(false),
		mFrameLimit(0)
	{
	}
}
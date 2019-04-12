#pragma once

#include "Types.h"

#include <string>

namespace JonsEngine
{
	struct WindowSettings
	{
		std::string mWindowTitle;
		uVec2 mWindowDimensions;
		bool mFullscreen;
		uint16_t mFrameLimit;


		WindowSettings();
		WindowSettings(const std::string& windowTitle, uint32_t width, uint32_t height, bool fullscreen, uint16_t frameLimit);
	};

	inline WindowSettings::WindowSettings() :
		WindowSettings("JonsEngine Game", 1920, 1080, false, 0)
	{
	}

	inline WindowSettings::WindowSettings(const std::string& windowTitle, uint32_t width, uint32_t height, bool fullscreen, uint16_t frameLimit) :
		mWindowTitle(windowTitle),
		mWindowDimensions(width, height),
		mFullscreen(fullscreen),
		mFrameLimit(frameLimit)
	{
	}
}
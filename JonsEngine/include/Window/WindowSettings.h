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
		WindowSettings(const std::string& windowTitle, uint16_t width, uint16_t height, bool fullscreen, uint16_t frameLimit);
	};

	inline WindowSettings::WindowSettings() :
		WindowSettings("JonsEngine Game", 1920, 1080, false, 0)
	{
	}

	inline WindowSettings::WindowSettings(const std::string& windowTitle, uint16_t width, uint16_t height, bool fullscreen, uint16_t frameLimit) :
		mWindowTitle(windowTitle),
		mWindowWidth(width),
		mWindowHeight(height),
		mFullscreen(fullscreen),
		mFrameLimit(frameLimit)
	{
	}
}
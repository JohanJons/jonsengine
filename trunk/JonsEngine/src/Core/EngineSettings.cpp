#include "include/Core/EngineSettings.h"

namespace JonsEngine
{
	EngineSettings::EngineSettings() : mFullscreen(false), mWindowTitle("JonsEngine Game"), mScreenHeight(800), mScreenWidth(600), mFrameLimit(60)
	{
	}

	void EngineSettings::SetFullscreen(bool fullscreen)
	{
		mFullscreen = fullscreen;
	}
		
	void EngineSettings::SetWindowTitle(const std::string& title)
	{
		mWindowTitle = title;
	}
		
	void EngineSettings::SetScreenHeight(uint32_t height)
	{
		mScreenHeight = height;
	}
		
	void EngineSettings::SetScreenWidth(uint32_t width)
	{
		mScreenWidth = width;
	}
		
	void EngineSettings::SetFrameLimit(uint16_t frameLimit)
	{
		mFrameLimit = frameLimit;
	}

}
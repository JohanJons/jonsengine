#include "include/Core/EngineSettings.h"

namespace JonsEngine
{
	EngineSettings::EngineSettings() : mRenderBackend(OPENGL), mWindowTitle("JonsEngine Game")
	{
	}

	void EngineSettings::SetFullscreen(bool fullscreen)
	{
		mScreenMode.Fullscreen = fullscreen;
	}
		
	void EngineSettings::SetWindowTitle(const std::string& title)
	{
		mWindowTitle = title;
	}
		
	void EngineSettings::SetScreenHeight(uint16_t height)
	{
		mScreenMode.ScreenHeight = height;
	}
		
	void EngineSettings::SetScreenWidth(uint16_t width)
	{
		mScreenMode.ScreenWidth = width;
	}
		
	void EngineSettings::SetFrameLimit(uint16_t frameLimit)
	{
		mScreenMode.FrameLimit = frameLimit;
	}

	void EngineSettings::SetFrameLimitEnabled(bool useFrameLimit)
	{
		mScreenMode.FrameLimitEnabled = useFrameLimit;
	}


	RenderBackend EngineSettings::GetRenderBackend() const			{ return mRenderBackend;			}
	bool EngineSettings::IsFullscreen() const						{ return mScreenMode.Fullscreen;	}
	const std::string& EngineSettings::GetWindowTitle() const		{ return mWindowTitle;				}
	uint32_t EngineSettings::GetScreenHeight() const				{ return mScreenMode.ScreenHeight;	}
	uint32_t EngineSettings::GetScreenWidth() const					{ return mScreenMode.ScreenWidth;	}
	uint16_t EngineSettings::GetFrameLimit() const					{ return mScreenMode.FrameLimit;	}
	const ScreenMode& EngineSettings::GetScreenMode() const			{ return mScreenMode;				}

}
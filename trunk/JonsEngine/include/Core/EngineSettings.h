#pragma once

#include <string>

#include "include/Core/EngineDefs.h"
#include "include/Video/RenderBase.h"

namespace JonsEngine
{
	enum RenderBackend
	{
		OPENGL = 0,
		NONE
	};

	class EngineSettings
	{
	public:
		EngineSettings();

		// Video
		void SetRenderBackend(RenderBackend backend);
		void SetFullscreen(bool fullscreen);
		void SetWindowTitle(const std::string& title);
		void SetScreenHeight(uint16_t height);
		void SetScreenWidth(uint16_t width);
		void SetFrameLimit(uint16_t frameLimit);
		void SetFrameLimitEnabled(bool useFrameLimit);

		RenderBackend GetRenderBackend() const;
		bool IsFullscreen() const;
		const std::string& GetWindowTitle() const;
		uint32_t GetScreenHeight() const;
		uint32_t GetScreenWidth() const;
		uint16_t GetFrameLimit() const;
		bool IsFrameLimitEnabled() const;
		const ScreenMode& GetScreenMode() const;


		// Audio

		// Input


	private:
		RenderBackend mRenderBackend;
		ScreenMode mScreenMode;
		std::string mWindowTitle;
	};

}
#pragma once

#include <string>

#include "include/Core/EngineDefs.h"

namespace JonsEngine
{

	class EngineSettings
	{
	public:
		EngineSettings();

		// Video
		void SetFullscreen(bool fullscreen);
		void SetWindowTitle(const std::string& title);
		void SetScreenHeight(uint32_t height);
		void SetScreenWidth(uint32_t width);
		void SetFrameLimit(uint16_t frameLimit);

		inline bool IsFullscreen() const						{ return mFullscreen; }
		inline const std::string& GetWindowTitle() const		{ return mWindowTitle; }
		inline uint32_t GetScreenHeight() const					{ return mScreenHeight; }
		inline uint32_t GetScreenWidth() const					{ return mScreenWidth; }
		inline uint16_t GetFrameLimit() const					{ return mFrameLimit; }


		// Audio

		// Input


	private:
		bool mFullscreen;
		std::string mWindowTitle;
		uint32_t mScreenHeight;
		uint32_t mScreenWidth;
		uint16_t mFrameLimit;
	};

}
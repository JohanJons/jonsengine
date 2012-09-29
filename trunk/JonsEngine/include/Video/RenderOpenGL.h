#pragma once

#include "include/Video/RenderBase.h"

namespace JonsEngine
{
	class Logger;

	class RenderOpenGL : public RenderBase
	{
	public:
		RenderOpenGL();
		~RenderOpenGL();

		bool Init(const EngineSettings& engineSettings);
		void Destroy();

		bool SetupWindow(const ScreenMode& screenMode);
		void CloseWindow();
		bool IsWindowOpened() const;

		void SetFullscreen(bool fullscreen);
		void SetScreenResolution(const uint16_t width, const uint16_t height);
		void SetWindowTitle(const std::string& windowTitle);

		void StartFrame();
		void EndFrame();

	private:
		bool InitializeGLFW();
		bool InitializeGLEW();

		Logger& mLogger;
		ScreenMode mScreenMode;
		std::string mWindowTitle;

	};
}
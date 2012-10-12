#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Utils/Types.h"

namespace JonsEngine
{
	class EngineSettings;

	struct ScreenMode
	{
		ScreenMode();

		uint16_t ScreenWidth;
		uint16_t ScreenHeight;
		uint16_t FrameLimit;
		bool FrameLimitEnabled;
		bool Fullscreen;
	};


	class RenderBase
	{
	public:
		RenderBase();
		virtual ~RenderBase();

		virtual bool Init(const EngineSettings& engineSettings) = 0;
		virtual void Destroy() = 0;

		virtual void StartFrame() = 0;
		virtual void EndFrame() = 0;

		virtual bool SetupWindow(const ScreenMode& screenMode) = 0;
		virtual void CloseWindow() = 0;
		virtual bool IsWindowOpened() const = 0;
		virtual uint16_t GetCurrentFPS() const = 0;

		virtual void SetFullscreen(bool fullscreen) = 0;
		virtual void SetScreenResolution(const uint16_t width, const uint16_t height) = 0;
		virtual void SetWindowTitle(const std::string& windowTitle) = 0;

		virtual void RenderVertexArrays() = 0;
		virtual void DrawLine(const Vec3& pointA, const Vec3& pointB) = 0;
		virtual void DrawTriangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC) = 0;
		virtual void DrawRectangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC, const Vec3& pointD) = 0;


		inline bool IsRunning() const	                         { return mRunning;		}
		inline const ScreenMode& GetScreenMode() const					{ return mScreenMode;	}
		inline const std::string& GetWindowTitle() const				{ return mWindowTitle;	}
			

	protected:
		bool mRunning;
		ScreenMode mScreenMode;
		std::string mWindowTitle;
	};

}
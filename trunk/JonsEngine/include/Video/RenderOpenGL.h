#pragma once

#include "include/Video/RenderBase.h"
#include "include/Core/Containers/Vector.h"

#include "GL/glew.h"

namespace JonsEngine
{
	class Logger;

	struct PrimitiveInfo
	{
		GLenum Mode;
		GLsizei Count;
	};

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
		uint16_t GetCurrentFPS() const;

		void SetFullscreen(bool fullscreen);
		void SetScreenResolution(const uint16_t width, const uint16_t height);
		void SetWindowTitle(const std::string& windowTitle);

		void RenderVertexArrays();
		void DrawLine(const Vec3& pointA, const Vec3& pointB);
		void DrawTriangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC);
		void DrawRectangle(const Vec3& pointA, const Vec3& pointB, const Vec3& pointC, const Vec3& pointD);

		void StartFrame();
		void EndFrame();

	private:
		bool InitializeGLFW();
		bool InitializeGLEW();

		Logger& mLogger;
		ScreenMode mScreenMode;
		std::string mWindowTitle;
		double mStartFrameTime;

		// FPS
		double mLastFrameTime;
		double mThisFrameTime;

		// rendering 
		GLuint mVertexBuffer;
		Vector<Vec3> mVertices;
		Vector<PrimitiveInfo> mPrimitiveInfo;


	};
}
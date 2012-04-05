#ifndef _RENDER_MANAGER_H
#define _RENDER_MANAGER_H

#ifdef ANDROID
#include "GLES2/gl2.h"
#elif defined _WIN32 || _WIN64
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <gl\GL.h>
#include <gl\GLU.h>
#endif

#include "../../Interface/IRenderManager.h"
#include "../../Interface/ILogManager.h"
#include "../../Interface/EngineDefs.h"

namespace JonsEngine
{

	class RenderManagerImpl : public IRenderManager
	{
	public:
		RenderManagerImpl(ILogManager* logger);
		~RenderManagerImpl();

		int32_t Init();
		bool Destroy();
		bool Start();
		bool Stop();
		bool isRunning();
		void Tick();

		void ResizeWindow(int32_t width, int32_t height);
		int32_t GetWindowWidth();
		int32_t GetWindowHeight();

	private:
		ILogManager* mLog;

		bool mRunning;
		int32_t mScreenWidth;
		int32_t mScreenHeight;
	};

}

#endif

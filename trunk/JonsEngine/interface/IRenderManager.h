#ifndef _I_RENDER_MANAGER_H
#define _I_RENDER_MANAGER_H

#include "EngineDefs.h"

namespace JonsEngine
{

	class IRenderManager
	{
	public:
		virtual ~IRenderManager() { }

		virtual int32_t Init() = 0;
		virtual bool Destroy() = 0;
		virtual bool Start() = 0;
		virtual bool Stop() = 0;
		virtual bool isRunning() = 0;
		virtual void Tick() = 0;

		virtual void ResizeWindow(int32_t width, int32_t height) = 0;
		virtual int32_t GetWindowWidth() = 0;
		virtual int32_t GetWindowHeight() = 0;
	};

}

#endif

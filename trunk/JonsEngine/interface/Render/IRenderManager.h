#ifndef _I_RENDER_MANAGER_H
#define _I_RENDER_MANAGER_H

#include "interface/Core/Logging/ILogManager.h"

#include "include/Core/EngineDefs.h"

namespace JonsEngine
{

	class IRenderManager
	{
	public:
		virtual ~IRenderManager() { }

		virtual bool Init(ILogManager* const logger, IMemoryManager* const memmgr) = 0;
		virtual bool Destroy() = 0;
		virtual bool Start() = 0;
		virtual bool Stop() = 0;
		virtual bool isRunning() = 0;
		virtual void Tick() = 0;

		virtual void ResizeWindow(int32_t width, int32_t height) = 0;
		virtual int32_t GetWindowWidth() const = 0;
		virtual int32_t GetWindowHeight() const = 0;
	};

}

#endif

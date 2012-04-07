#ifndef _I_MEMORY_MANAGER_H
#define _I_MEMORY_MANAGER_H

#include "EngineDefs.h"
#include "ILogManager.h"

namespace JonsEngine
{

	class IMemoryManager
	{
	public:
		virtual ~IMemoryManager() { }

		virtual bool Init(ILogManager* logger) = 0;
		virtual bool Init(bool UseDLMalloc, ILogManager* logger) = 0;
		virtual bool Destroy() = 0;
		virtual bool Start() = 0;
		virtual bool Stop() = 0;
		virtual bool isRunning() = 0;
		virtual void Tick() = 0;

		virtual void* Allocate(size_t size) = 0;
		virtual void* ReAllocate(void* p, size_t n) = 0;
		virtual void Deallocate(void* obj) = 0;

		virtual uint64_t GetTotalAllocatedObjectsCount() = 0;
		virtual uint64_t GetCurrentAllocatedObjectsCount() = 0;
	};

}

#endif

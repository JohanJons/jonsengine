#ifndef _I_MEMORY_MANAGER_H
#define _I_MEMORY_MANAGER_H

#include "interface/Core/EngineDefs.h"

namespace JonsEngine
{
	class ILogManager;
	class HeapAllocator;

	class IMemoryManager
	{
	public:
		virtual ~IMemoryManager() { }

		virtual bool Init(ILogManager* logger) = 0;
		virtual bool Destroy() = 0;
		virtual bool Start() = 0;
		virtual bool Stop() = 0;
		virtual bool isRunning() = 0;
		virtual void Tick() = 0;

		virtual HeapAllocator& GetHeapAllocator()  = 0;
		virtual uint64_t GetTotalAllocatedMemory() const = 0;
	};

}

#endif

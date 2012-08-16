#ifndef _MEMORY_MANAGER_IMPL_H
#define _MEMORY_MANAGER_IMPL_H


#include "interface/Core/Memory/IMemoryManager.h"
#include "interface/Core/Logging/ILogManager.h"

#include "include/Core/EngineDefs.h"
#include "include/Core/Memory/HeapAllocator.h"

namespace JonsEngine
{

	class MemoryManagerImpl : public IMemoryManager
	{
	public:
		MemoryManagerImpl();
		~MemoryManagerImpl();

		bool Init(ILogManager* const logger);
		bool Destroy();
		bool Start();
		bool Stop();
		bool isRunning();
		void Tick();

		inline HeapAllocator* const GetHeapAllocator() { return &mHeapAllocator; }
		uint64_t GetTotalAllocatedMemory() const;

	private:
		void* InternalAllocate(size_t size);
		void InternalDeAllocate(void* p);

		ILogManager* mLog;
		HeapAllocator mHeapAllocator;
		bool mRunning;
		bool mInitialized;

	};

}

#endif

#ifndef _JONS_THREAD_POOL_H
#define _JONS_THREAD_POOL_H

#include "interface/Core/Threading/IThreadPool.h"
#include "interface/Core/EngineDefs.h"

namespace JonsEngine
{
	class IMemoryAllocator;
	class ILogManager;

	class ThreadPool : public IThreadPool
	{
	public:
		ThreadPool(IMemoryAllocator& allocator, ILogManager& logger, uint32_t numThreads);
		~ThreadPool();

		void AddTask(Task task);
		void Wait();

		uint32_t GetNumThreads() const;

	private:
		IMemoryAllocator& mMemoryAllocator;
		ILogManager& mLogger;
		uint32_t mNumThreads;
	};
}

#endif
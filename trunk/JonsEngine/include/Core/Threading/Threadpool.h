#ifndef _JONS_THREAD_POOL_H
#define _JONS_THREAD_POOL_H

#include "interface/Core/Threading/IThreadPool.h"
#include "interface/Core/EngineDefs.h"

#include "include/Core/Containers/Vector.h"

namespace JonsEngine
{
	class IMemoryAllocator;
	class ILogManager;
	class IThreadingFactory;

	class ThreadPool : public IThreadPool
	{
	public:
		ThreadPool(IMemoryAllocator& allocator, ILogManager& logger, IThreadingFactory& factory, uint32_t initialNumThreads);
		~ThreadPool();

		void AddTask(Task task);
		void ClearTasks();
		size_t PendingTasks() const;
		bool Empty() const;
		void Wait();

		void SetNumThreads(uint32_t num);
		uint32_t GetNumThreads() const;

	private:
		void Worker(void* arg);

		IMemoryAllocator& mMemoryAllocator;
		ILogManager& mLogger;
		IThreadingFactory& mFactory;
		uint32_t mNumThreads;
		Vector<Task> mScheduledTasks;
	};
}

#endif
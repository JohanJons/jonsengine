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
	class IMutex;
	class IConditionVariable;

	class ThreadPool : public IThreadPool
	{
	public:
		ThreadPool(IMemoryAllocator& allocator, ILogManager& logger, IThreadingFactory& factory, uint32_t initialNumThreads);
		~ThreadPool();

		void AddTask(const Task& task);
		void ClearTasks();
		size_t PendingTasks() const;
		bool Empty() const;
		void Wait();
		void Wait(const size_t taskLimit);

		void SetNumThreads(uint32_t num);
		uint32_t GetNumThreads() const;

	private:
		void Worker(void* arg);

		IMemoryAllocator& mMemoryAllocator;
		ILogManager& mLogger;
		IThreadingFactory& mFactory;

		IMutex* mMutex;
		IConditionVariable* mCondVar_WorkDone;
		IConditionVariable* mCondVar_NewTaskOrKillWorker;
		Vector<Task> mScheduledTasks;
		uint32_t mNumThreads;
		uint32_t mDesiredNumThreads;
	};
}

#endif
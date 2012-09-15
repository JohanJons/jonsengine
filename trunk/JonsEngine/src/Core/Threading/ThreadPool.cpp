#include "include/Core/Threading/ThreadPool.h"

#include "interface/Core/Logging/ILogManager.h"
#include "interface/Core/Threading/IThreadingFactory.h"
#include "interface/Core/Threading/IMutex.h"
#include "interface/Core/Threading/IConditionVariable.h"

#include "include/Core/Threading/ScopedLock.h"

#include "boost/bind.hpp"

namespace JonsEngine
{
	ThreadPool::ThreadPool(IMemoryAllocator& allocator, ILogManager& logger, IThreadingFactory& factory, uint32_t initialNumThreads) : mMemoryAllocator(allocator), mLogger(logger), mFactory(factory), mMutex(NULL), 
																																	mNumThreads(0), mDesiredNumThreads(0), mScheduledTasks(allocator), mCondVar_WorkDoneOrWorkerKilled(NULL), mCondVar_NewTaskOrKillWorker(NULL)
	{
		mMutex = mFactory.CreateMutex();
		mCondVar_WorkDoneOrWorkerKilled = mFactory.CreateConditionVariable();
		mCondVar_NewTaskOrKillWorker = mFactory.CreateConditionVariable();

		SetNumThreads(initialNumThreads);
	}

	ThreadPool::~ThreadPool()
	{
		// order and block untill all workers are dead
		TerminateAllWorkers();

		if (mMutex)
			mFactory.DestroyMutex(mMutex);

		if (mCondVar_WorkDoneOrWorkerKilled)
			mFactory.DestroyConditionVariable(mCondVar_WorkDoneOrWorkerKilled);

		if (mCondVar_NewTaskOrKillWorker)
			mFactory.DestroyConditionVariable(mCondVar_NewTaskOrKillWorker);
	}

	void ThreadPool::AddTask(const Task& task)
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::AddTask(): Mutex is invalid" << std::endl;
			return;
		}

		if (!mCondVar_NewTaskOrKillWorker)
		{
			mLogger.LogError() << "ThreadPool::AddTask(): Condition Variable is invalid" << std::endl;
			return;
		}

		ScopedLock lock(mMutex);

		// schedule task and notify atleast one worker
		mScheduledTasks.push_back(task);
		mCondVar_NewTaskOrKillWorker->Signal();
	}

	void ThreadPool::ClearTasks()
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::AddTask(): Mutex is invalid" << std::endl;
			return;
		}

		ScopedLock lock(mMutex);

		mScheduledTasks.clear();
	}

	size_t ThreadPool::PendingTasks() const
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::PendingTasks(): Mutex is invalid" << std::endl;
			return 0;
		}

		ScopedLock lock(mMutex);

		size_t ret = mScheduledTasks.size();

		return ret;
	}

	bool ThreadPool::Empty() const
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::Empty(): Mutex is invalid" << std::endl;
			return false;
		}

		ScopedLock lock(mMutex);

		bool ret = mScheduledTasks.empty();

		return ret;
	}
	
	void ThreadPool::Wait()
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::Wait(): Mutex is invalid" << std::endl;
			return;
		}

		if (!mCondVar_WorkDoneOrWorkerKilled)
		{
			mLogger.LogError() << "ThreadPool::Wait(): Condition Variable is invalid" << std::endl;
			return;
		}

		while(!mScheduledTasks.empty())
		{
			ScopedLock lock(mMutex);

			mCondVar_WorkDoneOrWorkerKilled->Wait(mMutex);
		}
	}

	void ThreadPool::Wait(const size_t taskLimit)
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::Wait(): Mutex is invalid" << std::endl;
			return;
		}

		if (!mCondVar_WorkDoneOrWorkerKilled)
		{
			mLogger.LogError() << "ThreadPool::Wait(): Condition Variable is invalid" << std::endl;
			return;
		}

		while (mScheduledTasks.size() > taskLimit)
		{
			ScopedLock lock(mMutex);

			mCondVar_WorkDoneOrWorkerKilled->Wait(mMutex);
		}
	}

	void ThreadPool::SetNumThreads(uint32_t num)
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::SetNumThreads(): Mutex is invalid" << std::endl;
			return;
		}

		ScopedLock lock(mMutex);

		mDesiredNumThreads = num;

		if (mNumThreads < num)
		{
			while (mNumThreads < num)
			{
				mFactory.CreateThread(boost::bind(&ThreadPool::Worker, this, (void*)NULL));
				mNumThreads++;
			}
		}
		else
			mCondVar_NewTaskOrKillWorker->Broadcast();
	}

	uint32_t ThreadPool::GetNumThreads() const
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::GetNumThreads(): Mutex is invalid" << std::endl;
			return 0;
		}

		ScopedLock lock(mMutex);

		uint32_t ret = mNumThreads;

		return ret;
	}

	void ThreadPool::Worker(void* arg)
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::Worker(): Mutex is invalid" << std::endl;
			return;
		}

		if (!mCondVar_WorkDoneOrWorkerKilled || !mCondVar_NewTaskOrKillWorker)
		{
			mLogger.LogError() << "ThreadPool::Worker(): Condition Variables are invalid" << std::endl;
			return;
		}

		bool running = true;

		while (running)
		{
			Task task = NULL;

			{
				ScopedLock lock(mMutex);

				if (mDesiredNumThreads < mNumThreads)
				{
					running = false;
					mNumThreads--;
				}
				else if (!mScheduledTasks.empty())
				{
					task = mScheduledTasks.front();
					mScheduledTasks.erase(mScheduledTasks.begin());
				}
				else
					mCondVar_NewTaskOrKillWorker->Wait(mMutex);
			}

			if (task)
			{
				task();
				mCondVar_WorkDoneOrWorkerKilled->Signal();
			}
			else if (!running)
				mCondVar_WorkDoneOrWorkerKilled->Signal();
		}
	}

	void ThreadPool::TerminateAllWorkers()
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::TerminateAllWorkers(): Mutex is invalid" << std::endl;
			return;
		}

		ScopedLock lock(mMutex);

		mDesiredNumThreads = 0;

		mCondVar_NewTaskOrKillWorker->Broadcast();

		while (GetNumThreads())
			mCondVar_WorkDoneOrWorkerKilled->Wait(mMutex);
	}
}
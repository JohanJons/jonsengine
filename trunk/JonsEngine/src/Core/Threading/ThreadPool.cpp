#include "include/Core/Threading/ThreadPool.h"

#include "interface/Core/Logging/ILogManager.h"
#include "interface/Core/Threading/IThreadingFactory.h"
#include "interface/Core/Threading/IMutex.h"
#include "interface/Core/Threading/IConditionVariable.h"

#include "boost/bind.hpp"

namespace JonsEngine
{
	ThreadPool::ThreadPool(IMemoryAllocator& allocator, ILogManager& logger, IThreadingFactory& factory, uint32_t initialNumThreads) : mMemoryAllocator(allocator), mLogger(logger), mFactory(factory), mMutex(NULL), 
																																	mNumThreads(0), mDesiredNumThreads(0), mScheduledTasks(allocator), mCondVar_WorkDone(NULL), mCondVar_NewTaskOrKillWorker(NULL)
	{
		mMutex = mFactory.CreateMutex();
		mCondVar_WorkDone = mFactory.CreateConditionVariable();
		mCondVar_NewTaskOrKillWorker = mFactory.CreateConditionVariable();

		SetNumThreads(initialNumThreads);
	}

	ThreadPool::~ThreadPool()
	{
		if (mMutex)
			mFactory.DestroyMutex(mMutex);

		if (mCondVar_WorkDone)
			mFactory.DestroyConditionVariable(mCondVar_WorkDone);

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

		mMutex->Lock();

		mScheduledTasks.push_back(task);
		mCondVar_NewTaskOrKillWorker->Signal();

		mMutex->Unlock();
	}

	void ThreadPool::ClearTasks()
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::AddTask(): Mutex is invalid" << std::endl;
			return;
		}

		mMutex->Lock();

		mScheduledTasks.clear();

		mMutex->Unlock();
	}

	size_t ThreadPool::PendingTasks() const
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::PendingTasks(): Mutex is invalid" << std::endl;
			return 0;
		}

		mMutex->Lock();

		size_t ret = mScheduledTasks.size();

		mMutex->Unlock();


		return ret;
	}

	bool ThreadPool::Empty() const
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::Empty(): Mutex is invalid" << std::endl;
			return false;
		}

		mMutex->Lock();

		bool ret = mScheduledTasks.empty();

		mMutex->Unlock();


		return ret;
	}
	
	void ThreadPool::Wait()
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::Wait(): Mutex is invalid" << std::endl;
			return;
		}

		if (!mCondVar_WorkDone)
		{
			mLogger.LogError() << "ThreadPool::Wait(): Condition Variable is invalid" << std::endl;
			return;
		}

		bool waiting = true;

		while (waiting)
		{
			mMutex->Lock();

			bool isEmpty = mScheduledTasks.empty();

			mMutex->Unlock();

			if (isEmpty)
			{
				waiting = false;
			}
			else
				mCondVar_WorkDone->Wait();
		}
	}

	void ThreadPool::Wait(const size_t taskLimit)
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::Wait(): Mutex is invalid" << std::endl;
			return;
		}

		if (!mCondVar_WorkDone)
		{
			mLogger.LogError() << "ThreadPool::Wait(): Condition Variable is invalid" << std::endl;
			return;
		}

		mMutex->Lock();

		size_t tasksLeft = mScheduledTasks.size();

		mMutex->Unlock();

		while (tasksLeft > taskLimit)
		{
			mCondVar_WorkDone->Wait();

			mMutex->Lock();

			tasksLeft = mScheduledTasks.size();

			mMutex->Unlock();
		}
	}

	void ThreadPool::SetNumThreads(uint32_t num)
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::SetNumThreads(): Mutex is invalid" << std::endl;
			return;
		}

		mMutex->Lock();

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

		mMutex->Unlock();
	}

	uint32_t ThreadPool::GetNumThreads() const
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::GetNumThreads(): Mutex is invalid" << std::endl;
			return 0;
		}

		mMutex->Lock();

		uint32_t ret = mNumThreads;

		mMutex->Unlock();


		return ret;
	}

	void ThreadPool::Worker(void* arg)
	{
		if (!mMutex)
		{
			mLogger.LogError() << "ThreadPool::Worker(): Mutex is invalid" << std::endl;
			return;
		}

		if (!mCondVar_WorkDone || !mCondVar_NewTaskOrKillWorker)
		{
			mLogger.LogError() << "ThreadPool::Worker(): Condition Variables are invalid" << std::endl;
			return;
		}

		bool running = true;

		while (running)
		{
			mMutex->Lock();

			Task task = NULL;

			// either terminate the worker, or check for tasks
			if (mDesiredNumThreads < mNumThreads)
			{
				running = false;
			}
			else
			{
				if (!mScheduledTasks.empty())
				{
					task = mScheduledTasks.front();
					mScheduledTasks.erase(mScheduledTasks.begin());
				}
				
			
			}
		}
	}
}
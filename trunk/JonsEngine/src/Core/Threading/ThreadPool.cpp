#include "include/Core/Threading/ThreadPool.h"

#include "include/Core/Threading/Mutex.h"
#include "include/Core/Threading/ConditionVariable.h"
#include "include/Core/Threading/ScopedLock.h"
#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/bind.hpp"

namespace JonsEngine
{
    ThreadPool::ThreadPool(uint32_t initialNumThreads) : mMemoryAllocator(HeapAllocator::GetDefaultHeapAllocator()), mLogger(Logger::GetCoreLogger()),
                                                            mNumThreads(0), mDesiredNumThreads(0), mScheduledTasks(HeapAllocator::GetDefaultHeapAllocator()),
                                                            mWorkerThreads(HeapAllocator::GetDefaultHeapAllocator())
    {
        SetNumThreads(initialNumThreads);
    }

    ThreadPool::~ThreadPool()
    {
        // order and block untill all workers are dead
        TerminateAllWorkers();

        for(Vector<Thread*>::iterator it = mWorkerThreads.begin(); it != mWorkerThreads.end();)
        {
            mMemoryAllocator.DeallocateObject<Thread>(*it);
            it = mWorkerThreads.erase(it);
        }
    }

    void ThreadPool::AddTask(const Task& task)
    {
        ScopedLock lock(mMutex);

        // schedule task and notify atleast one worker
        mScheduledTasks.push_back(task);
        mCondVar_NewTaskOrKillWorker.Signal();
    }

    void ThreadPool::ClearTasks()
    {
        ScopedLock lock(mMutex);

        mScheduledTasks.clear();
    }

    size_t ThreadPool::PendingTasks() const
    {
        ScopedLock lock(mMutex);

        size_t ret = mScheduledTasks.size();

        return ret;
    }

    bool ThreadPool::Empty() const
    {
        ScopedLock lock(mMutex);

        bool ret = mScheduledTasks.empty();

        return ret;
    }
    
    void ThreadPool::Wait()
    {
        while(!mScheduledTasks.empty())
        {
            ScopedLock lock(mMutex);

            mCondVar_WorkDoneOrWorkerKilled.Wait(mMutex);
        }
    }

    void ThreadPool::Wait(const size_t taskLimit)
    {
        while (mScheduledTasks.size() > taskLimit)
        {
            ScopedLock lock(mMutex);

            mCondVar_WorkDoneOrWorkerKilled.Wait(mMutex);
        }
    }

    void ThreadPool::SetNumThreads(uint32_t num)
    {
        ScopedLock lock(mMutex);

        if (mDesiredNumThreads == num)
            return;

        mDesiredNumThreads = num;

        if (mNumThreads < num)
        {
            while (mNumThreads < num)
            {
                Thread* thread = mMemoryAllocator.AllocateObject<Thread>(boost::bind(&ThreadPool::Worker, this, (void*)NULL));
                mWorkerThreads.push_back(thread);
                mNumThreads++;
            }
        }
        else
            mCondVar_NewTaskOrKillWorker.Broadcast();
    }

    uint32_t ThreadPool::GetNumThreads() const
    {
        ScopedLock lock(mMutex);

        uint32_t ret = mNumThreads;

        return ret;
    }

    void ThreadPool::Worker(void* arg)
    {
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
                    mCondVar_NewTaskOrKillWorker.Wait(mMutex);
            }

            if (task)
            {
                task();
                mCondVar_WorkDoneOrWorkerKilled.Signal();
            }
            else if (!running)
                mCondVar_WorkDoneOrWorkerKilled.Signal();
        }
    }

    void ThreadPool::TerminateAllWorkers()
    {
        ScopedLock lock(mMutex);

        mDesiredNumThreads = 0;

        mCondVar_NewTaskOrKillWorker.Broadcast();

        while (GetNumThreads())
            mCondVar_WorkDoneOrWorkerKilled.Wait(mMutex);
    }
}
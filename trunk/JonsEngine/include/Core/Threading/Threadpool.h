#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Threading/Thread.h"
#include "include/Core/Threading/Mutex.h"
#include "include/Core/Threading/ConditionVariable.h"
#include "include/Core/Containers/vector.h"

namespace JonsEngine
{
    class IMemoryAllocator;
    class Logger;

    /* ThreadPool definition */
    class ThreadPool
    {
    public:
        ThreadPool(uint32_t initialNumThreads);
        ~ThreadPool();

        void AddTask(const Task& task);
        void ClearTasks();
        size_t PendingTasks() const;
        bool Empty() const;
        void Wait();

        /*
         * Waits untill there are only a given number of tasks left
         */
        void Wait(const size_t taskLimit);

        /*
         * Sets the number of active threads
         */
        void SetNumThreads(uint32_t num);

        /*
         * Gets the number of active threads
         */
        uint32_t GetNumThreads() const;


    private:
        void Worker(void* arg);
        void TerminateAllWorkers();

        IMemoryAllocator& mMemoryAllocator;
        Logger& mLogger;

        mutable Mutex mMutex;
        ConditionVariable mCondVar_WorkDoneOrWorkerKilled;
        ConditionVariable mCondVar_NewTaskOrKillWorker;

        vector<Task> mScheduledTasks;
        vector<Thread*> mWorkerThreads;
        uint32_t mNumThreads;
        uint32_t mDesiredNumThreads;
    };
}
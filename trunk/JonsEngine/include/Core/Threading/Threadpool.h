#ifndef _JONS_THREAD_POOL_H
#define _JONS_THREAD_POOL_H

#include "include/Core/EngineDefs.h"
#include "include/Core/Threading/Thread.h"
#include "include/Core/Threading/Mutex.h"

namespace JonsEngine
{
	class ThreadPool
	{
	public:
		ThreadPool(uint32_t numThreads);
		~ThreadPool();

		void AddTask();
		void Wait();

		uint32_t GetNumThreads() const;

	private:
	};
}

#endif
#ifndef _JONS_THREAD_POOL_H
#define _JONS_THREAD_POOL_H

#include "../../../interface/EngineDefs.h"

#include "Thread.h"
#include "Mutex.h"

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
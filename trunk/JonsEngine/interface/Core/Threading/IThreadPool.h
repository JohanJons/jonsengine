#ifndef _JONS_ITHREAD_POOL_H
#define _JONS_ITHREAD_POOL_H

#include "interface/Core/Threading/IThread.h"
#include "interface/Core/EngineDefs.h"

namespace JonsEngine
{
	class IThreadPool
	{
	public:
		virtual ~IThreadPool() { }

		virtual void AddTask(Task task) = 0;
		virtual void ClearTasks() = 0;
		virtual size_t PendingTasks() const = 0;
		virtual bool Empty() const = 0;
		virtual void Wait() = 0;

		virtual void SetNumThreads(uint32_t num) = 0;
		virtual uint32_t GetNumThreads() const = 0;
	};

}

#endif
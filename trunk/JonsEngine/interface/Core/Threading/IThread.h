#ifndef _JONS_ITHREAD_H
#define _JONS_ITHREAD_H

#include "include/Core/EngineDefs.h"
#include "include/Core/Memory/HeapAllocator.h"

namespace JonsEngine
{
	/* Thread wrapper class */
	class IThread
	{
	public:
		#if defined _WIN32 || _WIN64
			typedef void* ThreadHandle;  
		#else
			#include <pthread.h>
			typedef pthread_t ThreadHandle;
		#endif

		enum ThreadState
		{
			DETACHED = 0,
			RUNNING,
			FINISHED
		};

		virtual ~IThread() { }

		virtual int32_t Join() = 0;
		virtual int32_t SetPriority(int32_t priority) = 0;

		virtual ThreadHandle& GetNativeHandle() = 0;
		virtual ThreadState GetThreadState() const = 0;

	};


	/* Free-standing functions */
	void jons_SleepCurrentThread(uint32_t milliseconds);
	IThread::ThreadHandle jons_GetCurrentThreadNativeHandle();
	int32_t jons_SetThreadPriority(int32_t priority);
	int32_t jons_SetThreadPriority(IThread::ThreadHandle handle, int32_t priority);
}

#endif
#ifndef _JONS_THREAD_H
#define _JONS_THREAD_H

#include "../../../interface/EngineDefs.h"

#include "../../Core/Memory/HeapAllocator.h"

namespace JonsEngine
{
	#if defined _WIN32 || _WIN64
		typedef void* ThreadHandle;  
	#else
		#include <pthread.h>
		typedef pthread_t ThreadHandle;
	#endif


	/* Free-standing functions */
	void jons_SleepCurrentThread(uint32_t milliseconds);
	ThreadHandle jons_GetCurrentThreadNativeHandle();
	int32_t jons_SetThreadPriority(int32_t priority);
	int32_t jons_SetThreadPriority(ThreadHandle handle, int32_t priority);


	/* Thread wrapper class */
	class Thread
	{
	public:
		enum ThreadState
		{
			DETACHED = 0,
			RUNNING,
			FINISHED
		};

		struct ThreadInfo
		{
			ThreadState mState;
			void* (*mFunctionPointer) (void*);
			void* mArg;
		};

		Thread();
		Thread(void* (*start) (void*), void* arg, IMemoryAllocator* const allocator);
		~Thread();

		Thread& operator=(Thread& other);
		int32_t Join();
		int32_t SetPriority(int32_t priority);
		ThreadHandle GetNativeHandle();
		ThreadState GetThreadState();

	private:
		ThreadHandle mHandle;
		ThreadInfo* mThreadInfo;
		IMemoryAllocator* mAllocator;

		int32_t Destroy();
		int32_t Detach();
		static void* Run(void* arg);
		ThreadHandle _CreateThread(void* (*start) (void*), void* arg);
		void _JoinThread(ThreadHandle& handle);

	};
}

#endif
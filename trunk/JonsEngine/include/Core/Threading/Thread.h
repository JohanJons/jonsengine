#ifndef _JONS_THREAD_H
#define _JONS_THREAD_H

#include "../../../interface/EngineDefs.h"

namespace JonsEngine
{
	#if defined _WIN32 || _WIN64
		#include <Windows.h>
		typedef uint64_t ThreadHandle;  
	#else
		#include <pthread.h>
		typedef pthread_t ThreadHandle;
	#endif

	ThreadHandle SpawnThread(void* (*start) (void*), void* arg);
	ThreadHandle SpawnThread(void* (*start) (void*), void* arg, bool suspended);
	void SleepThread(uint64_t milliseconds);
	void JoinThread();
	void JoinThread(uint64_t timeoutMilliseconds);
	void SetThreadPriority(int32_t priority);
	void SetThreadPriority(ThreadHandle handle, int32_t priority);
	void DestroyThread(ThreadHandle handle);


}

#endif
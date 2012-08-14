#ifndef _JONS_MUTEX_H
#define _JONS_MUTEX_H

#if defined _WIN32 || _WIN64
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#else
	#include <pthread.h>
#endif

#include "../../../interface/EngineDefs.h"

namespace JonsEngine
{
	class Mutex
	{
	public:
		enum MutexState
		{
			UNLOCKED = 0,
			LOCKED
		};

		#if defined _WIN32 || _WIN64
			typedef HANDLE MutexHandle;
		#else
			typedef pthread_mutex_t MutexHandle;
		#endif

		Mutex();
		~Mutex();

		int32_t Lock();
		int32_t Unlock();

		MutexHandle& GetNativeHandle();
		const MutexState& GetMutexState() const;

	private:
		MutexHandle mHandle;
		MutexState mState;
	};
}

#endif
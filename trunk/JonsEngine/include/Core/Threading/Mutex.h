#ifndef _JONS_MUTEX_H
#define _JONS_MUTEX_H

#include "interface/Core/Threading/IMutex.h"

#if defined _WIN32 || _WIN64
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#elif ANDROID
#include <pthread.h>
#endif

namespace JonsEngine
{
	#if defined _WIN32 || _WIN64
		typedef CRITICAL_SECTION MutexHandle;
	#else
		typedef pthread_mutex_t MutexHandle;
	#endif

	class ILogManager;

	class Mutex : public IMutex
	{
	public:
		Mutex(ILogManager& logger);
		~Mutex();

		int32_t Lock();
		int32_t Unlock();

		const MutexState& GetMutexState() const;
		MutexHandle& GetMutexHandle();

	private:
		MutexHandle mHandle;
		MutexState mState;
		ILogManager& mLogger;
	};
}

#endif
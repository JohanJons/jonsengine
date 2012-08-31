#ifndef _JONS_MUTEX_H
#define _JONS_MUTEX_H

#ifdef ANDROID
#include <pthread.h>
#endif

#include "interface/Core/Threading/IMutex.h"

namespace JonsEngine
{
	class ILogManager;

	class Mutex : public IMutex
	{
	public:
		#if defined _WIN32 || _WIN64
			typedef void* MutexHandle;
		#else
			typedef pthread_mutex_t MutexHandle;
		#endif

		Mutex(ILogManager& logger);
		~Mutex();

		int32_t Lock();
		int32_t Unlock();

		const MutexState& GetMutexState() const;

	private:
		MutexHandle mHandle;
		MutexState mState;
		ILogManager& mLogger;
	};
}

#endif
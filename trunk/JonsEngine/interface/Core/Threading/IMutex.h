#ifndef _JONS_IMUTEX_H
#define _JONS_IMUTEX_H

#if defined _WIN32 || _WIN64
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#else
	#include <pthread.h>
#endif

#include "interface/Core/EngineDefs.h"

namespace JonsEngine
{
	class IMutex
	{
	public:
		enum MutexState
		{
			UNLOCKED = 0,
			LOCKED
		};

		#if defined _WIN32 || _WIN64
			typedef void* MutexHandle;
		#else
			typedef pthread_mutex_t MutexHandle;
		#endif

		virtual ~IMutex() { }

		virtual int32_t Lock() = 0;
		virtual int32_t Unlock() = 0;

		virtual MutexHandle& GetNativeHandle() = 0;
		virtual const MutexState& GetMutexState() const = 0;
	};
}

#endif
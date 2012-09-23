#pragma once

#include "include/Core/Threading/Mutex.h"

namespace JonsEngine
{
	class Mutex;

	class ScopedLock
	{
	public:
		inline ScopedLock(Mutex& mutex) : mMutex(mutex)		{ mMutex.Lock(); }
		inline ~ScopedLock()								{ mMutex.Unlock(); }

	private:
		Mutex& mMutex;
	};
}
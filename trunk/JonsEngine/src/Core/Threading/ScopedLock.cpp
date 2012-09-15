#include "include/Core/Threading/ScopedLock.h"

#include "interface/Core/Threading/IMutex.h"

namespace JonsEngine
{
	/*
	 *
	 * Scoped lock
	 *
	 */
	ScopedLock::ScopedLock(IMutex* mutex) : mMutex(mutex)
	{
		mMutex->Lock();
	}

	ScopedLock::~ScopedLock()
	{
		mMutex->Unlock();
	}
}
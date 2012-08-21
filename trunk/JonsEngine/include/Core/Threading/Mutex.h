#ifndef _JONS_MUTEX_H
#define _JONS_MUTEX_H

#include "interface/Core/Threading/IMutex.h"
#include "interface/Core/Logging/ILogManager.h"

namespace JonsEngine
{
	class Mutex : public IMutex
	{
	public:
		Mutex(ILogManager& logger);
		~Mutex();

		int32_t Lock();
		int32_t Unlock();

		MutexHandle& GetNativeHandle();
		const MutexState& GetMutexState() const;

	private:
		MutexHandle mHandle;
		MutexState mState;
		ILogManager& mLogger;
	};
}

#endif
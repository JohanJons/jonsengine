#ifndef _JONS_IMUTEX_H
#define _JONS_IMUTEX_H

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

		virtual ~IMutex() { }

		virtual int32_t Lock() = 0;
		virtual int32_t Unlock() = 0;

		virtual const MutexState& GetMutexState() const = 0;
	};
}

#endif
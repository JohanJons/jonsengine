#ifndef _JONS_ITHREAD_POOL_H
#define _JONS_ITHREAD_POOL_H

#include "interface/Core/Threading/IThread.h"
#include "interface/Core/Threading/IMutex.h"
#include "interface/Core/Threading/IConditionVariable.h"

#include "include/Core/EngineDefs.h"

namespace JonsEngine
{
	class IThreadPool
	{
	public:
		virtual ~IThreadPool() { }


	};

}

#endif
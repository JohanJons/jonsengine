#ifndef _JONS_ICONDITION_VARIABLE_H
#define _JONS_ICONDITION_VARIABLE_H

#if defined _WIN32 || _WIN64
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#undef WIN32_LEAN_AND_MEAN
#else
	#include <pthread.h>
	#include <time.h>
#endif

#include "interface/Core/EngineDefs.h"
#include "interface/Core/Threading/IMutex.h"
#include "interface/Core/Logging/ILogManager.h"

namespace JonsEngine
{
	class IConditionVariable
	{
	public:
		#if defined _WIN32 || _WIN64
			typedef HANDLE ConditionVariableHandle;
		#else
			typedef pthread_cond_t ConditionVariableHandle;
		#endif

		enum ConditionVariableState
		{
			READY,
			WAITING
		};

		virtual ~IConditionVariable() { }

		virtual void Wait() = 0;
		virtual void TimedWait(uint32_t milliseconds) = 0;
		virtual void Signal() = 0;
		virtual void Broadcast() = 0;

		virtual ConditionVariableHandle& GetNativeConditionVariableHandle() = 0;
		virtual const ConditionVariableState& GetConditionVariableState() const = 0;
	};
}

#endif
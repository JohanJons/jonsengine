#ifndef _JONS_ICONDITION_VARIABLE_H
#define _JONS_ICONDITION_VARIABLE_H

#include "interface/Core/EngineDefs.h"

namespace JonsEngine
{
	class IMutex;

	class IConditionVariable
	{
	public:
		enum ConditionVariableState
		{
			READY,
			WAITING
		};

		virtual ~IConditionVariable() { }

		virtual void Wait(IMutex* mutex) = 0;
		virtual void TimedWait(IMutex* mutex, uint32_t milliseconds) = 0;
		virtual void Signal() = 0;
		virtual void Broadcast() = 0;

		virtual const ConditionVariableState& GetConditionVariableState() const = 0;
	};
}

#endif
#ifndef _JONS_ITHREAD_H
#define _JONS_ITHREAD_H

#include "interface/Core/EngineDefs.h"

#include "boost/function.hpp"

namespace JonsEngine
{
	/* Task definition */
	typedef boost::function0<void> Task;

	/* Thread wrapper class */
	class IThread
	{
	public:
		enum ThreadState
		{
			DETACHED = 0,
			RUNNING,
			FINISHED
		};

		virtual ~IThread() { }

		IThread& operator=(IThread& other);
		virtual int32_t Join() = 0;
		virtual int32_t SetPriority(int32_t priority) = 0;

		virtual ThreadState GetThreadState() const = 0;

	};


	/* Free-standing functions */
	void jons_SleepCurrentThread(uint32_t milliseconds);
	int32_t jons_SetThreadPriority(int32_t priority);
}

#endif
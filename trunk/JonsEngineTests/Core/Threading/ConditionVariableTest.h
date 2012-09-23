#ifndef _COND_VAR_TEST_H
#define _COND_VAR_TEST_H

#include "gtest/gtest.h"

#include "include/Core/Threading/Thread.h"
#include "include/Core/Threading/ConditionVariable.h"
#include "include/Core/Threading/ScopedLock.h"

#include "../JonsEngineTests/TestFixtures/RunningEngineFixture.h"


namespace JonsEngine
{
	class ConditionVariableTest : public RunningEngineFixture
	{
	public:
		ConditionVariableTest()
		{
			
		}

		virtual void SetUp()
		{
			RunningEngineFixture::SetUp();
		}

		virtual void TearDown()
		{
			RunningEngineFixture::TearDown();
		}

		void Increment(int32_t* intPtr, bool* waiting, Mutex* mutex, ConditionVariable* condVar)
		{ 
			ScopedLock lock(*mutex);

			while (*waiting)
				condVar->Wait(*mutex);

			(*intPtr)++;
		}

		void Signal(int32_t* intPtr, bool* waiting, Mutex* mutex, ConditionVariable* condVar)
		{
			ScopedLock lock(*mutex);

			(*intPtr)++;

			*waiting = false;

			condVar->Signal();
		}

		void timedWait100ms(Mutex* mutex, ConditionVariable* condVar)
		{
			ScopedLock lock(*mutex);

			condVar->TimedWait(*mutex, 100);
		}
	};

}

#endif
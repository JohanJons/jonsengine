#ifndef _MUTEX_TEST_H
#define _MUTEX_TEST_H

#include "gtest/gtest.h"

#include "include/Core/Threading/Thread.h"
#include "include/Core/Threading/Mutex.h"

#include "../JonsEngineTests/TestFixtures/RunningEngineFixture.h"


namespace JonsEngine
{
	class MutexTest : public RunningEngineFixture
	{
	public:
		MutexTest()
		{
			
		}

		static void incrementer(IMutex* mutex)
		{
			mutex->Lock();
			mCount++;
			mutex->Unlock();
		}

		virtual void SetUp()
		{
			mCount = 0;

			RunningEngineFixture::SetUp();
		}

		virtual void TearDown()
		{
			RunningEngineFixture::TearDown();
		}

		static int mCount;

	};

}

#endif
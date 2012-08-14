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

		static void* incrementer(void* arg)
		{
			Mutex* mutex = static_cast<Mutex*>(arg);

			mutex->Lock();
			mCount++;
			mutex->Unlock();

			return NULL;
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
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

		static void incrementer(Mutex* mutex, int32_t* count)
		{
			mutex->Lock();
			(*count)++;
			mutex->Unlock();
		}

		virtual void SetUp()
		{
			RunningEngineFixture::SetUp();
		}

		virtual void TearDown()
		{
			RunningEngineFixture::TearDown();
		}

	};

}

#endif
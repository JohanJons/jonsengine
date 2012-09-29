#ifndef _MUTEX_TEST_H
#define _MUTEX_TEST_H

#include "gtest/gtest.h"

#include "include/Core/Threading/Thread.h"
#include "include/Core/Threading/Mutex.h"

#include "../JonsEngineTests/TestFixtures/RunningEngineFixture.h"


namespace JonsEngine
{
	class MutexTest : public ::testing::Test
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
	};

}

#endif
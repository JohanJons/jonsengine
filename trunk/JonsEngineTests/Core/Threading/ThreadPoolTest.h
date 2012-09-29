#ifndef _MUTEX_TEST_H
#define _MUTEX_TEST_H

#include "gtest/gtest.h"

#include "../JonsEngineTests/TestFixtures/RunningEngineFixture.h"

#include "include/Core/Threading/ThreadPool.h"

#include "boost/bind.hpp"


namespace JonsEngine
{
	class ThreadPoolTest : public ::testing::Test
	{
	public:
		ThreadPoolTest()
		{
		}

		void AddTasks(ThreadPool& pool, uint32_t numTasks)
		{
			for (uint32_t i = 0; i < numTasks; i++)
				pool.AddTask(boost::bind(&ThreadPoolTest::Arbeit, this));
		}

		void Arbeit()
		{
			SleepCurrentThread(20);
		}
	};

}

#endif
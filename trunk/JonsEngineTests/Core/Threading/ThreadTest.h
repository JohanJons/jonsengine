#ifndef _THREAD_TEST_H
#define _THREAD_TEST_H

#include "gtest/gtest.h"

#include "include/Core/Threading/Thread.h"

#include "../JonsEngineTests/TestFixtures/RunningEngineFixture.h"


namespace JonsEngine
{
	class ThreadTest : public RunningEngineFixture
	{
	public:
		ThreadTest()
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

		void SetNumberTo14(int32_t* number)
		{
			*number = 14;
		}

		void Sleeper(int32_t* arg)
		{
			int32_t sleepTime = *arg;

			jons_SleepCurrentThread(sleepTime);
		}
	};
}




#endif
#ifndef _MUTEX_TEST_H
#define _MUTEX_TEST_H

#include "gtest/gtest.h"

#include "../JonsEngineTests/TestFixtures/RunningEngineFixture.h"


namespace JonsEngine
{
	class ThreadPoolTest : public RunningEngineFixture
	{
	public:
		ThreadPoolTest()
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

	};

}

#endif
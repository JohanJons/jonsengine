#ifndef _RUNNING_ENGINE_FIXTURE_H
#define _RUNNING_ENGINE_FIXTURE_H

#include "gtest/gtest.h"

#include "include/Core/Engine.h"

namespace JonsEngine
{
	 class RunningEngineFixture : public ::testing::Environment
	 {
	 public:
		 virtual void SetUp()
		 {
			// init engine
			ASSERT_TRUE(mEngine.Init());

			ASSERT_TRUE(mEngine.isRunning());
		 }
		
		 virtual void TearDown()
		 {
			// destroy engine
			ASSERT_TRUE(mEngine.Destroy());

			ASSERT_FALSE(mEngine.isRunning());
		 }

	 protected:
		 Engine mEngine;
	 };

}

#endif
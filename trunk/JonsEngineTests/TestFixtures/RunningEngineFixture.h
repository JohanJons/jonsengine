#ifndef _RUNNING_ENGINE_FIXTURE_H
#define _RUNNING_ENGINE_FIXTURE_H

#include "gtest/gtest.h"

#include "Engine.h"

namespace JonsEngine
{
	 class RunningEngineFixture : public ::testing::Test
	 {
	 public:
		 virtual void SetUp()
		 {
			mEngine.GetEngineSettings().SetLogToFile(false);
			mEngine.GetEngineSettings().SetLogToSTDOut(false);

			// init engine
			ASSERT_TRUE(mEngine.Init());

			// start engine
			ASSERT_TRUE(mEngine.Start());

			ASSERT_TRUE(mEngine.isRunning());
		 }
		
		 virtual void TearDown()
		 {
			// stop engine
			ASSERT_TRUE(mEngine.Stop());

			// destroy engine
			ASSERT_TRUE(mEngine.Destroy());
		 }

	 protected:
		 Engine mEngine;
	 };

}

#endif
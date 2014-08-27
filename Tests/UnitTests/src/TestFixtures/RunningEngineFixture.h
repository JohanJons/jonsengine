#ifndef _RUNNING_ENGINE_FIXTURE_H
#define _RUNNING_ENGINE_FIXTURE_H

#include "gtest/gtest.h"

#include "include/Core/Engine.h"
#include "include/Core/EngineSettings.h"

namespace JonsEngine
{
	 class RunningEngineFixture : public ::testing::Environment
	 {
	 public:
		 virtual void SetUp()
		 {
             mEngine = new Engine(mSettings);
		 }
		
		 virtual void TearDown()
		 {
            delete mEngine;
		 }

	 protected:
         static EngineSettings mSettings;
		 static Engine* mEngine;
	 };
}

#endif
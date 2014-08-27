#ifndef _SCENE_MANAGER_TEST_H
#define _SCENE_MANAGER_TEST_H

#include "gtest/gtest.h"

#include "../TestFixtures/RunningEngineFixture.h"


namespace JonsEngine
{
	class SceneManagerTest : public ::testing::Test, public RunningEngineFixture
	{
	public:
		SceneManagerTest()
		{
		}
	};

}

#endif
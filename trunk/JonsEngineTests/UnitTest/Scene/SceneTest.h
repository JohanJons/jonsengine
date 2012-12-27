#ifndef _SCENE_TEST_H
#define _SCENE_TEST_H

#include "gtest/gtest.h"

#include "../TestFixtures/RunningEngineFixture.h"


namespace JonsEngine
{
	class SceneTest : public ::testing::Test, public RunningEngineFixture
	{
	public:
		SceneTest()
		{
		}
	};

}

#endif
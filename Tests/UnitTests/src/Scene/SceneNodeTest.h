#ifndef _SCENE_NODE_TEST_H
#define _SCENE_NODE_TEST_H

#include "gtest/gtest.h"

#include "../TestFixtures/RunningEngineFixture.h"


namespace JonsEngine
{
	class SceneNodeTest : public ::testing::Test, public RunningEngineFixture
	{
	public:
		SceneNodeTest()
		{
		}
	};

}

#endif
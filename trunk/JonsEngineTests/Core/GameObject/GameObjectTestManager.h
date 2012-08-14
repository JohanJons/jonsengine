#ifndef _GAMEOBJECT_MANAGER_TEST_H
#define _GAMEOBJECT_MANAGER_TEST_H

#include "include/Core/Engine.h"
#include "include/Core/GameObject/GameObject.h"
#include "include/Core/Utils/JonsTime.h"

#include "../JonsEngineTests/TestClass1.h"

#define JONSOBJECT_ITERATIONS 1000

namespace JonsEngine
{
	class GameObjectTestManager
	{
	public:
		GameObjectTestManager(IGameObjectManager* mGameObjFact,Engine* engine);

		/* Object tests */
		bool ReferenceTest();

	private:
		/* Aux functions */
		bool GameObjectRefTest1();
		bool GameObjectRefTest2(boost::shared_ptr<TestClass1> test1);
		bool GameObjectRefTest3(boost::shared_ptr<TestClass1> test1, boost::shared_ptr<TestClass1> test2);

		IGameObjectManager* mGameObjManager;
		Engine* mEngine;

	};

}

#endif

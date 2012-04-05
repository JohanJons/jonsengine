#ifndef _TEST_CLASS_1_H
#define _TEST_CLASS_1_H

#include "../JonsEngine/include/Core/GameObject/GameObject.h"

namespace JonsEngine
{

	class TestClass1 : public GameObject
	{
	public:
		TestClass1();
		TestClass1(bool UseMemPool);
		~TestClass1();

		void TestMe();
	};

}

#endif

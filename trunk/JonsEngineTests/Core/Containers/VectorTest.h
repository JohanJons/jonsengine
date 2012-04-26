#ifndef _VECTOR_TEST_H
#define _VECTOR_TEST_H

#include "gtest/gtest.h"

#include "Core/Containers/Vector.h"

#include "../../TestFixtures/RunningEngineFixture.h"


namespace JonsEngine
{

	class VectorTest : public RunningEngineFixture
	{
	public:
		VectorTest() : mVector1(mEngine.GetMemoryManager()->GetHeapAllocator()),
						mVector2(mEngine.GetMemoryManager()->GetHeapAllocator(),5),
						mVector3(mEngine.GetMemoryManager()->GetHeapAllocator(),10)
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

		Vector<int> mVector1;
		Vector<int> mVector2;
		Vector<int> mVector3;
	};

}




#endif
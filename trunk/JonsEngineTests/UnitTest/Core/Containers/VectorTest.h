#ifndef _VECTOR_TEST_H
#define _VECTOR_TEST_H

#include "gtest/gtest.h"

#include "include/Core/Containers/Vector.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "../../TestFixtures/RunningEngineFixture.h"


namespace JonsEngine
{

	class VectorTest : public ::testing::Test
	{
	public:
		VectorTest() : mVector1(Globals::GetDefaultHeapAllocator()),
						mVector2(Globals::GetDefaultHeapAllocator(), 5),
						mVector3(Globals::GetDefaultHeapAllocator(), 10)
		{

		}

		Vector<int32_t> mVector1;
		Vector<int32_t> mVector2;
		Vector<int32_t> mVector3;
	};

}




#endif
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
		VectorTest() : mVector1(HeapAllocator::GetDefaultHeapAllocator()),
						mVector2(HeapAllocator::GetDefaultHeapAllocator(), 5),
						mVector3(HeapAllocator::GetDefaultHeapAllocator(), 10)
		{

		}

		Vector<int32_t> mVector1;
		Vector<int32_t> mVector2;
		Vector<int32_t> mVector3;
	};

}




#endif
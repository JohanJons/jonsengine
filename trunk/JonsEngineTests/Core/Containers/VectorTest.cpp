#include "VectorTest.h"


namespace JonsEngine
{
	/**
	 * Capacity test
	 */
	TEST_F(VectorTest, capacity)
	{
		ASSERT_EQ(0,mVector1.capacity());
		ASSERT_EQ(5,mVector2.capacity());
		ASSERT_EQ(10,mVector3.capacity());

		mVector1.push_back(1);
		ASSERT_EQ(mVector1.DefaultCapacityIncrease,mVector1.capacity());
		mVector1.pop_back();
		ASSERT_EQ(mVector1.DefaultCapacityIncrease,mVector1.capacity());

		mVector2.insert(mVector2.begin(),5,1);
		ASSERT_EQ(5,mVector2.capacity());
		mVector2.push_back(1);
		ASSERT_EQ(10,mVector2.capacity());
		mVector2.reset();
		ASSERT_EQ(0,mVector2.capacity());

		mVector3.resize(6);
		ASSERT_EQ(6,mVector3.capacity());
	}


	/**
	 * size test
	 */
	TEST_F(VectorTest, size)
	{
		ASSERT_EQ(0,mVector1.size());
		ASSERT_EQ(0,mVector2.size());
		ASSERT_EQ(0,mVector3.size());

		mVector1.push_back(1);
		ASSERT_EQ(1,mVector1.size());
		mVector1.pop_back();
		ASSERT_EQ(0,mVector1.size());

		mVector2.insert(mVector2.begin(),5,1);
		ASSERT_EQ(5,mVector2.size());
		mVector2.push_back(1);
		ASSERT_EQ(6,mVector2.size());
		mVector2.reset();
		ASSERT_EQ(0,mVector2.size());

		mVector3.resize(6);
		ASSERT_EQ(0,mVector3.size());
		mVector3.insert(mVector3.begin(),13,1);
		ASSERT_EQ(13,mVector3.size());
		mVector3.resize(10);
		ASSERT_EQ(10,mVector3.size());
		mVector3.erase();
		mVector3.clear();
		ASSERT_EQ(0,mVector3.size());

	}




}
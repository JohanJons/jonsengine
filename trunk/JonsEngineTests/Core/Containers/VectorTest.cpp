#include "VectorTest.h"


namespace JonsEngine
{
	/**
	 * begin test
	 */
	TEST_F(VectorTest, begin)
	{
		mVector1.push_back(1);
		mVector1.push_back(2);
		mVector1.push_back(3);
		ASSERT_EQ(3,mVector1.size());

		Vector<int>::Iterator iter = mVector1.begin();
		ASSERT_EQ(*iter++,1);
		ASSERT_EQ(*iter++,2);
		ASSERT_EQ(*iter,3);

	}

	/**
	 * end test
	 */
	TEST_F(VectorTest, end)
	{
		mVector1.push_back(1);
		mVector1.push_back(2);
		mVector1.push_back(3);
		ASSERT_EQ(3,mVector1.size());

		Vector<int>::Iterator iter = mVector1.end();
		ASSERT_EQ(*--iter,3); 
		ASSERT_EQ(*--iter,2);
		ASSERT_EQ(*--iter,1);
	}

	/**
	 * front test
	 */
	TEST_F(VectorTest, front)
	{
		mVector1.push_back(1);
		mVector1.push_back(2);
		mVector1.push_back(3);
		ASSERT_EQ(3,mVector1.size());

		ASSERT_EQ(mVector1.front(),1); 
	}

	/**
	 * back test
	 */
	TEST_F(VectorTest, back)
	{
		mVector1.push_back(1);
		mVector1.push_back(2);
		mVector1.push_back(3);
		ASSERT_EQ(3,mVector1.size());

		ASSERT_EQ(mVector1.back(),3); 
	}

	/**
	 * at test
	 */
	TEST_F(VectorTest, at)
	{
		mVector1.push_back(1);
		mVector1.push_back(2);
		mVector1.push_back(3);
		ASSERT_EQ(3,mVector1.size());

		ASSERT_EQ(mVector1.at(0),1); 
		ASSERT_EQ(mVector1.at(1),2); 
		ASSERT_EQ(mVector1.at(2),3); 
	}

	/**
	 * operator[] test
	 */
	TEST_F(VectorTest, operatorBrackets)
	{
		mVector1.push_back(1);
		mVector1.push_back(2);
		mVector1.push_back(3);
		ASSERT_EQ(3,mVector1.size());

		ASSERT_EQ(mVector1[0],1); 
		ASSERT_EQ(mVector1[1],2); 
		ASSERT_EQ(mVector1[2],3); 
	}

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
		mVector3.erase(mVector3.begin(),mVector3.begin()+3);
		ASSERT_EQ(7,mVector3.size());
		mVector3.clear();
		ASSERT_EQ(0,mVector3.size());

	}

	/**
	 * empty test
	 */
	TEST_F(VectorTest, empty)
	{
		ASSERT_EQ(true,mVector3.empty());

		mVector1.push_back(1);
		mVector1.push_back(2);
		mVector1.push_back(3);
		ASSERT_EQ(3,mVector1.size());
		ASSERT_EQ(false,mVector1.empty());

		mVector1.pop_back();
		mVector1.pop_back();
		mVector1.pop_back();
		ASSERT_EQ(true,mVector1.empty());

		mVector1.insert(mVector1.begin(),12,1);
		ASSERT_EQ(false,mVector1.empty());

		mVector1.clear();
		ASSERT_EQ(true,mVector1.empty());

		mVector1.insert(mVector1.begin(),23,1);
		ASSERT_EQ(false,mVector1.empty());

		mVector1.erase(mVector1.begin(),mVector1.end());
		ASSERT_EQ(true,mVector1.empty());
	}

	/**
	 * clear test
	 */
	TEST_F(VectorTest, clear)
	{
		mVector1.push_back(1);
		mVector1.push_back(2);
		mVector1.push_back(3);
		ASSERT_EQ(3,mVector1.size());
		mVector1.clear();
		ASSERT_EQ(true,mVector1.empty());
	}

	/**
	 * reset test
	 */
	TEST_F(VectorTest, reset)
	{
		ASSERT_EQ(0,mVector1.capacity());

		mVector1.push_back(1);
		mVector1.push_back(2);
		mVector1.push_back(3);
		ASSERT_EQ(4,mVector1.capacity());

		mVector1.reset();
		ASSERT_EQ(0,mVector1.capacity());
	}

	/**
	 * insert test
	 */
	TEST_F(VectorTest, insert)
	{
		// insert(Iterator position, const T& value)
		ASSERT_EQ(0,mVector1.capacity());
		ASSERT_EQ(0,mVector1.size());

		mVector1.insert(mVector1.begin(),1);
		ASSERT_EQ(1,mVector1.size());
		ASSERT_EQ(1,*mVector1.begin());

		mVector1.insert(mVector1.begin(),2);
		ASSERT_EQ(2,mVector1.size());
		ASSERT_EQ(2,*mVector1.begin());
		ASSERT_EQ(1,*(mVector1.begin()+1));

		mVector1.insert(mVector1.begin()+1,3);
		ASSERT_EQ(3,mVector1.size());
		ASSERT_EQ(2,*mVector1.begin());
		ASSERT_EQ(3,*(mVector1.begin()+1));
		ASSERT_EQ(1,*(mVector1.begin()+2));

		mVector1.insert(mVector1.end(),4);
		ASSERT_EQ(4,mVector1.size());
		ASSERT_EQ(2,*mVector1.begin());
		ASSERT_EQ(3,*(mVector1.begin()+1));
		ASSERT_EQ(1,*(mVector1.begin()+2));
		ASSERT_EQ(4,*(mVector1.end()-1));

		mVector1.reset();
		ASSERT_EQ(0,mVector1.capacity());
		ASSERT_EQ(0,mVector1.size());

		// insert(Iterator position,size_t n, const T& value)
		mVector1.insert(mVector1.begin(),0,1);
		ASSERT_EQ(0,mVector1.size());
		ASSERT_EQ(0,mVector1.capacity());

		mVector1.insert(mVector1.begin(),5,1);
		ASSERT_EQ(5,mVector1.size());
		for (Vector<int>::Iterator i = mVector1.begin(); i != mVector1.end(); i++)
			ASSERT_EQ(*i,1);

		mVector1.insert(mVector1.begin(),2,2);
		ASSERT_EQ(7,mVector1.size());
		for (Vector<int>::Iterator i = mVector1.begin(); i != mVector1.begin()+2; i++)
			ASSERT_EQ(*i,2);
		for (Vector<int>::Iterator i = mVector1.begin()+2; i != mVector1.end(); i++)
			ASSERT_EQ(*i,1);

		mVector1.insert(mVector1.begin()+2,2,3);
		ASSERT_EQ(9,mVector1.size());
		for (Vector<int>::Iterator i = mVector1.begin(); i != mVector1.begin()+2; i++)
			ASSERT_EQ(*i,2);
		for (Vector<int>::Iterator i = mVector1.begin()+2; i != mVector1.begin()+4; i++)
			ASSERT_EQ(*i,3);
		for (Vector<int>::Iterator i = mVector1.begin()+4; i != mVector1.end(); i++)
			ASSERT_EQ(*i,1);

		mVector1.insert(mVector1.end(),5,4);
		ASSERT_EQ(14,mVector1.size());
		for (Vector<int>::Iterator i = mVector1.begin(); i != mVector1.begin()+2; i++)
			ASSERT_EQ(*i,2);
		for (Vector<int>::Iterator i = mVector1.begin()+2; i != mVector1.begin()+4; i++)
			ASSERT_EQ(*i,3);
		for (Vector<int>::Iterator i = mVector1.begin()+4; i != mVector1.begin()+9; i++)
			ASSERT_EQ(*i,1);
		for (Vector<int>::Iterator i = mVector1.begin()+9; i != mVector1.end(); i++)
			ASSERT_EQ(*i,4);

		mVector1.reset();
		ASSERT_EQ(0,mVector1.capacity());
		ASSERT_EQ(0,mVector1.size());

		// insert(Iterator position,Iterator first, Iterator last)
		mVector1.insert(mVector1.begin(),mVector1.begin(),mVector1.end());
		ASSERT_EQ(0,mVector1.size());
		ASSERT_EQ(0,mVector1.capacity());

		mVector1.push_back(1);
		mVector1.insert(mVector1.begin(),mVector1.begin(),mVector1.end());
		ASSERT_EQ(2,mVector1.size());
		ASSERT_EQ(2,mVector1.capacity());
		ASSERT_EQ(1,mVector1[0]);
		ASSERT_EQ(1,mVector1[1]);

		mVector1.insert(mVector1.end(),mVector1.begin(),mVector1.end());
		ASSERT_EQ(4,mVector1.size());
		ASSERT_EQ(4,mVector1.capacity());
		for (Vector<int>::Iterator i = mVector1.begin(); i < mVector1.end(); i++)
			ASSERT_EQ(1,*i);

		mVector1.insert(mVector1.begin(),mVector1.begin()+2,mVector1.end());
		ASSERT_EQ(6,mVector1.size());
		ASSERT_EQ(8,mVector1.capacity());
		for (Vector<int>::Iterator i = mVector1.begin(); i < mVector1.end(); i++)
			ASSERT_EQ(1,*i);


	}

	/**
	 * erase test
	 */
	TEST_F(VectorTest, erase)
	{
		// Iterator erase(Iterator position);
		mVector1.insert(mVector1.begin(),5,1);
		ASSERT_EQ(5,mVector1.size());
		ASSERT_EQ(mVector1.begin(),mVector1.erase(mVector1.begin()));
		ASSERT_EQ(4,mVector1.size());
		ASSERT_EQ(mVector1.end(),mVector1.erase(mVector1.end()-1));
		ASSERT_EQ(3,mVector1.size());

		// Iterator erase(Iterator first, Iterator last);
		ASSERT_EQ(mVector1.end(),mVector1.erase(mVector1.begin()+1,mVector1.end()));
		ASSERT_EQ(1,mVector1.size());
	}

	/*
		void resize(size_t numElements);
		*/


}
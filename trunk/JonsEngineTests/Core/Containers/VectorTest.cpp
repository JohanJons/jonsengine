#include "../JonsEngineTests/Core/Containers/VectorTest.h"


namespace JonsEngine
{
	/**
	 * copy constructor test
	 */
	TEST_F(VectorTest, copyConstructor)
	{
		mVector1.insert(mVector1.begin(),10,1);
		Vector<int> vecCopy(mVector1);

		ASSERT_EQ(mVector1.size(), vecCopy.size());
		ASSERT_EQ(mVector1.capacity(), vecCopy.capacity());
		for (size_t i = 0; i < vecCopy.size(); i++)
			ASSERT_EQ(mVector1[i], vecCopy[i]);
	}

	/**
	 * operator= test
	 */
	TEST_F(VectorTest, operatorAssign)
	{
		mVector1.insert(mVector1.begin(),10,1);
		mVector2 = mVector1;
		for (size_t i = 0; i < mVector2.size(); i++)
			ASSERT_EQ(mVector2[i], mVector1[i]);
		ASSERT_EQ(mVector1.capacity(), mVector2.capacity());
		ASSERT_EQ(mVector1.GetAllocator(), mVector2.GetAllocator());

		mVector1 = Vector<int>(mEngine.GetMemoryManager()->GetHeapAllocator());
		ASSERT_EQ(0,mVector1.size());
		ASSERT_EQ(0,mVector1.capacity());
		for (size_t i = 0; i < mVector1.size(); i++)
			FAIL();
	}

	/**
	 * GetAllocator test
	 */
	TEST_F(VectorTest, GetAllocator)
	{
		ASSERT_EQ(mEngine.GetMemoryManager()->GetHeapAllocator(),mVector1.GetAllocator());
	}

	/**
	 * begin test
	 */
	TEST_F(VectorTest, begin)
	{
		mVector1.push_back(1);
		mVector1.push_back(2);
		mVector1.push_back(3);
		ASSERT_EQ(3,mVector1.size());

		Vector<int>::iterator iter = mVector1.begin();
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

		Vector<int>::iterator iter = mVector1.end();
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
		ASSERT_EQ(10,mVector3.capacity());
		mVector3.resize(12);
		ASSERT_EQ(12,mVector3.capacity());
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
		ASSERT_EQ(6,mVector3.size());
		mVector3.insert(mVector3.begin(),13,1);
		ASSERT_EQ(19,mVector3.size());
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
	 * assign test
	 */
	TEST_F(VectorTest, assign)
	{
		ASSERT_EQ(0,mVector1.size());
		ASSERT_EQ(0,mVector2.size());

		mVector1.assign(7,1);
		ASSERT_EQ(7,mVector1.size());
		for (Vector<int>::iterator iter = mVector1.begin(); iter != mVector1.end(); iter++)
			ASSERT_EQ(1,*iter);

		mVector2.assign(mVector1.begin()+1,mVector1.end());
		ASSERT_EQ(6,mVector2.size());
		for (Vector<int>::iterator iter = mVector2.begin(); iter != mVector2.end(); iter++)
			ASSERT_EQ(1,*iter);

		mVector3.assign(11,1);
		ASSERT_EQ(11,mVector3.size());
		for (Vector<int>::iterator iter = mVector3.begin(); iter != mVector3.end(); iter++)
			ASSERT_EQ(1,*iter);

		mVector1.reset();
		mVector2.reset();
		ASSERT_EQ(0,mVector1.size());
		ASSERT_EQ(0,mVector2.size());
		ASSERT_EQ(0,mVector1.capacity());
		ASSERT_EQ(0,mVector2.capacity());
		mVector1.assign(mVector2.begin(),mVector2.end());
		ASSERT_EQ(0,mVector1.size());
		ASSERT_EQ(0,mVector1.capacity());
		for (size_t i = 0; i < mVector1.size(); i++)
			FAIL();


	}

	/**
	 * insert test
	 */
	TEST_F(VectorTest, insert)
	{
		// insert(iterator position, const T& value)
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

		// insert(iterator position,size_t n, const T& value)
		mVector1.insert(mVector1.begin(),0,1);
		ASSERT_EQ(0,mVector1.size());
		ASSERT_EQ(0,mVector1.capacity());

		mVector1.insert(mVector1.begin(),5,1);
		ASSERT_EQ(5,mVector1.size());
		for (Vector<int>::iterator i = mVector1.begin(); i != mVector1.end(); i++)
			ASSERT_EQ(*i,1);

		mVector1.insert(mVector1.begin(),2,2);
		ASSERT_EQ(7,mVector1.size());
		for (Vector<int>::iterator i = mVector1.begin(); i != mVector1.begin()+2; i++)
			ASSERT_EQ(*i,2);
		for (Vector<int>::iterator i = mVector1.begin()+2; i != mVector1.end(); i++)
			ASSERT_EQ(*i,1);

		mVector1.insert(mVector1.begin()+2,2,3);
		ASSERT_EQ(9,mVector1.size());
		for (Vector<int>::iterator i = mVector1.begin(); i != mVector1.begin()+2; i++)
			ASSERT_EQ(*i,2);
		for (Vector<int>::iterator i = mVector1.begin()+2; i != mVector1.begin()+4; i++)
			ASSERT_EQ(*i,3);
		for (Vector<int>::iterator i = mVector1.begin()+4; i != mVector1.end(); i++)
			ASSERT_EQ(*i,1);

		mVector1.insert(mVector1.end(),5,4);
		ASSERT_EQ(14,mVector1.size());
		for (Vector<int>::iterator i = mVector1.begin(); i != mVector1.begin()+2; i++)
			ASSERT_EQ(*i,2);
		for (Vector<int>::iterator i = mVector1.begin()+2; i != mVector1.begin()+4; i++)
			ASSERT_EQ(*i,3);
		for (Vector<int>::iterator i = mVector1.begin()+4; i != mVector1.begin()+9; i++)
			ASSERT_EQ(*i,1);
		for (Vector<int>::iterator i = mVector1.begin()+9; i != mVector1.end(); i++)
			ASSERT_EQ(*i,4);

		mVector1.insert(mVector1.begin()+4,130,5);
		ASSERT_EQ(144,mVector1.size());

		mVector1.insert(mVector1.begin()+76,531,6);
		ASSERT_EQ(675,mVector1.size());

		mVector1.insert(mVector1.end(),1005,7);
		ASSERT_EQ(1680,mVector1.size());

		mVector1.insert(mVector1.end(),10,8);
		ASSERT_EQ(1690,mVector1.size());

		mVector1.reset();
		ASSERT_EQ(0,mVector1.capacity());
		ASSERT_EQ(0,mVector1.size());

		// insert(iterator position,iterator first, iterator last)
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
		for (Vector<int>::iterator i = mVector1.begin(); i < mVector1.end(); i++)
			ASSERT_EQ(1,*i);

		mVector1.insert(mVector1.begin(),mVector1.begin()+2,mVector1.end());
		ASSERT_EQ(6,mVector1.size());
		ASSERT_EQ(8,mVector1.capacity());
		for (Vector<int>::iterator i = mVector1.begin(); i < mVector1.end(); i++)
			ASSERT_EQ(1,*i);


	}

	/**
	 * erase test
	 */
	TEST_F(VectorTest, erase)
	{
		// iterator erase(iterator position);
		mVector1.insert(mVector1.begin(),5,1);
		ASSERT_EQ(5,mVector1.size());
		ASSERT_EQ(mVector1.begin(),mVector1.erase(mVector1.begin()));
		ASSERT_EQ(4,mVector1.size());
		ASSERT_EQ(mVector1.end(),mVector1.erase(mVector1.end()-1));
		ASSERT_EQ(3,mVector1.size());

		// iterator erase(iterator first, iterator last);
		ASSERT_EQ(mVector1.end(),mVector1.erase(mVector1.begin()+1,mVector1.end()));
		ASSERT_EQ(1,mVector1.size());
		mVector1.insert(mVector1.begin(),5,1);
		ASSERT_EQ(6,mVector1.size());	
		ASSERT_EQ(mVector1.begin(),mVector1.erase(mVector1.begin(),mVector1.end()));
		ASSERT_EQ(0,mVector1.size());	
	}

	/**
	 * resize test
	 */
	TEST_F(VectorTest, resize)
	{
		mVector1.insert(mVector1.begin(),5,1);
		ASSERT_EQ(5,mVector1.size());
		mVector1.resize(3);
		ASSERT_EQ(3,mVector1.size());
		mVector1.resize(0);
		ASSERT_EQ(0,mVector1.size());
		mVector1.resize(21);
		ASSERT_EQ(21,mVector1.size());
		ASSERT_EQ(0,mVector1.at(16));
	}


}
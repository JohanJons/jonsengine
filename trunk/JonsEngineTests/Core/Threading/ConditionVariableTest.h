#ifndef _COND_VAR_TEST_H
#define _COND_VAR_TEST_H

#include "gtest/gtest.h"

#include "Core/Threading/Thread.h"
#include "Core/Threading/ConditionVariable.h"

#include "../../TestFixtures/RunningEngineFixture.h"


namespace JonsEngine
{
	class ConditionVariableTest : public RunningEngineFixture
	{
	public:
		ConditionVariableTest()
		{
			
		}

		virtual void SetUp()
		{
			mCount = 0;

			RunningEngineFixture::SetUp();
		}

		virtual void TearDown()
		{
			RunningEngineFixture::TearDown();
		}

		static void* signal(void* arg)
		{
			jons_SleepCurrentThread(50);

			mCount = 10;

			mCondVar.Signal();

			return NULL;
		}

		static void* setCountTo14(void* arg)
		{
			mCount = 14;	

			return NULL;
		}

		static void* timedWait500ms(void* arg)
		{
			mCondVar.TimedWait(500);

			return NULL;
		}

		static void* Incrementto4(void* arg)
		{
			for (;;)
			{
				if (mCount < 4)
				{
					mMutex.Lock();
					mCount++;
					if (mCount == 4)
					{
						mCondVar.Broadcast();
						return NULL;
					}
					mMutex.Unlock();
				}
				
				jons_SleepCurrentThread(50);
			}
		}

		static void* Incrementto7(void* arg)
		{
			for (;;)
			{
				if (mCount >= 4 && mCount < 7)
				{
					mMutex.Lock();
					mCount++;
					if (mCount == 7)
					{
						mCondVar.Broadcast();
						return NULL;
					}
					mMutex.Unlock();
				}
				else
					mCondVar.Wait();

				jons_SleepCurrentThread(50);
			}
		}

		static void* Incrementto10(void* arg)
		{
			for (;;)
			{
				if (mCount >= 7)
				{
					mMutex.Lock();
					mCount++;
					if (mCount == 10)
					{
						mCondVar.Broadcast();
						return NULL;
					}
					mMutex.Unlock();
				}
				else
					mCondVar.Wait();

				jons_SleepCurrentThread(50);
			}
		}

		static int mCount;
		static Mutex mMutex;
		static ConditionVariable mCondVar;

	};

}

#endif
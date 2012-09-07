#ifndef _COND_VAR_TEST_H
#define _COND_VAR_TEST_H

#include "gtest/gtest.h"

#include "include/Core/Threading/Thread.h"
#include "include/Core/Threading/ConditionVariable.h"

#include "../JonsEngineTests/TestFixtures/RunningEngineFixture.h"


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

		static void signal()
		{
			jons_SleepCurrentThread(50);

			mCount = 10;

			mCondVar->Signal();
		}

		static void setCountTo14()
		{
			mCount = 14;
		}

		static void timedWait500ms()
		{
			mCondVar->TimedWait(500);
		}

		static void Incrementto4()
		{
			for (;;)
			{
				if (mCount < 4)
				{
					mMutex->Lock();
					mCount++;
					if (mCount == 4)
					{
						mCondVar->Broadcast();
						return;
					}
					mMutex->Unlock();
				}
				
				jons_SleepCurrentThread(50);
			}
		}

		static void Incrementto7()
		{
			for (;;)
			{
				if (mCount >= 4 && mCount < 7)
				{
					mMutex->Lock();
					mCount++;
					if (mCount == 7)
					{
						mCondVar->Broadcast();
						return;
					}
					mMutex->Unlock();
				}
				else
					mCondVar->Wait();

				jons_SleepCurrentThread(50);
			}
		}

		static void Incrementto10()
		{
			for (;;)
			{
				if (mCount >= 7)
				{
					mMutex->Lock();
					mCount++;
					if (mCount == 10)
					{
						mCondVar->Broadcast();
						return;
					}
					mMutex->Unlock();
				}
				else
					mCondVar->Wait();

				jons_SleepCurrentThread(50);
			}
		}

		static int mCount;
		static IMutex* mMutex;
		static IConditionVariable* mCondVar;

	};

}

#endif
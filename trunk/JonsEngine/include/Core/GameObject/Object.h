/*#ifndef _JONS_OBJECT_H
#define _JONS_OBJECT_H

#include "../../../Interface/EngineDefs.h"
#include "../../../Interface/IMemoryManager.h"

namespace JonsEngine
{
	template <class T>
	class Object
	{
	public:
		inline Object(IMemoryManager* memMgr)
		{
			Init();

			if (memMgr)
			{
				mMemoryMgr = memMgr;

				mObj = (T*) mMemoryMgr->Allocate(sizeof(T));
				mRefCount = 1;
			}
		}

		inline Object(bool UseMemPool, IMemoryManager* memMgr)
		{
			Init();

			if (memMgr)
			{
				mMemoryMgr = memMgr;

				mObj = (T*) mMemoryMgr->Allocate(sizeof(T),UseMemPool);
				mRefCount = 1;
				mUseMemoryPool = UseMemPool;
			}
		}

		inline Object(Object<T>& ptr)
		{
			Init();

			mObj = ptr.mObj;
			mRefCount = ptr.mRefCount;
			mUseMemoryPool = ptr.mUseMemoryPool;
			mMemoryMgr = ptr.mMemoryMgr;
			IncRef();
		}

		inline ~Object()
		{
			if (mObj && mMemoryMgr)
			{
				DecRef();
				if (mRefCount <= 0)
				{
					mMemoryMgr->Deallocate(mObj,mUseMemoryPool);
				}
				mObj = NULL;
			}
		}

		inline void Init()
		{
			//mEngine = Engine::GetEngine();
			mUseMemoryPool = false;
		}

		inline void DecRef()
		{
			mRefCount--;
		}

		inline void IncRef()
		{
			mRefCount++;
		}

		inline void Release()
		{
			if (mObj && mEngine)
			{
				DecRef();
				if (mRefCount == 0)
				{
					mEngine->GetMemoryManager()->Deallocate(mObj,mUseMemoryPool);
				}
				mObj = NULL;
			}
		}

		inline T* Get()
		{
			if (mObj)
				return mObj;
			else
				return NULL;
		}

		inline void operator =(T* o)
		{
			Release();

			mObj = o;
			IncRef();
		}

		inline void operator =(Object &p)
		{
			Release();

			mObj = p.mObj;
			IncRef();
		}

		inline bool operator == (Object &p)
		{
			if (mObj)
				return mObj == p.mObj;
			else
				return false;
		}

		inline bool operator != (Object &p)
		{
			if (mObj != p.mObj)
				return true;
			else
				return false;
		}

		inline bool operator == (T* p )
		{
			if (mObj)
				return mObj == p;
			else
				return false;
		}

		inline bool operator != (T* p )
		{
			if (mObj != p)
				return true;
			else
				return false;
		}

		inline T* operator ->()
		{
			return Get();
		}

		inline T& operator *()
		{
			return *Get();
		}

		inline bool IsValid()
		{
			return (mObj != NULL);
		}

		inline bool IsAllocatedWithMemPool()
		{
			return mUseMemoryPool;
		}

		inline operator T *()
		{
			return Get();
		}


	protected:
		T* mObj;
		int32_t mRefCount;
		bool mUseMemoryPool;
		IMemoryManager* mMemoryMgr;
	};

}

#endif
*/
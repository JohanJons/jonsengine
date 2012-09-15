#ifndef _JONS_SCOPED_LOCK_H
#define _JONS_SCOPED_LOCK_H

namespace JonsEngine
{
	class IMutex;

	class ScopedLock
	{
	public:
		ScopedLock(IMutex* mutex);
		~ScopedLock();

	private:
		IMutex* mMutex;
	};
}

#endif
#ifndef _JONS_VECTOR_H
#define _JONS_VECTOR_H

#include "../../../Memory/Allocator.h"

namespace JonsEngine
{

	template <class T>
	class JonsVector<T>
	{
	public:
		JonsVector(Allocator* allocator);
		~JonsVector();

	private:
		T* mBegin;
		T* mEnd;
		Allocator* mAllocator;

	};

}

#endif
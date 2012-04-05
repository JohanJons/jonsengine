#ifndef _MEMORY_BLOCK_H
#define _MEMORY_BLOCK_H

//#include <malloc.h>

#include "../../../interface/EngineDefs.h"
//#include "../../../../Thirdparty/dlmalloc/dlmalloc.h"

template<class T>
class MemoryBlock
{
public:
	T* Begin();
	T* End();
	uint32_t NumberOfElements();
	uint32_t AvailableElementSlots();
	uint64_t AllocatedMemory();
	uint64_t AllocatedMemoryUsed();

};


#endif

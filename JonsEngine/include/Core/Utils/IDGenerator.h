#include "include/Core/Types.h"

#include <limits>

namespace JonsEngine
{
    // note: not thread-safe, make one if needed
	// note: does not keep track of active/inactive IDs, possible duplicates
    template <typename T>
    class IDGenerator
    {
    public:
        typedef uint32_t ID;
        static constexpr ID INVALID_ID = std::numeric_limits<ID>::max();
        
		static ID GetNextID();
        
    
    private:
        static constexpr ID FIRST_VALID_ID = std::numeric_limits<ID>::min();
        static constexpr ID mGlobalNext = FIRST_VALID_ID;
	};


	template <typename T>
	typename IDGenerator<T>::ID IDGenerator<T>::GetNextID()
	{
		return mGlobalNext == INVALID_ID ? FIRST_VALID_ID : mGlobalNext++;
	}
}
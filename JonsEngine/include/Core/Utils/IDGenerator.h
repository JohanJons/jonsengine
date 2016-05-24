#include "include/Core/Types.h"

#include <limits>

namespace JonsEngine
{
    // note: not thread-safe, make one if needed
    template <typename T>
    class IDGenerator
    {
    public:
        typedef uint32_t ID;
        static constexpr ID INVALID_ID = std::numeric_limits<ID>::max();
        
		static ID GetNextID() const;
        
    
    private:
        static constexpr ID FIRST_VALID_ID = std::numeric_limits<ID>::min();
        static mGlobalNext = FIRST_VALID_ID;
	};


	template <typename T>
	IDGenerator<T>::ID IDGenerator<T>::GetNextID() const
	{
		return mGlobalNext == INVALID_ID ? FIRST_VALID_ID : mGlobalNext++;
	}
}
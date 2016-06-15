#pragma once

#include "include/Core/Types.h"

#include <limits>
#include <queue>

namespace JonsEngine
{
	// note: max number of IDs registered at one time can be 65535
    template <typename T>
    class IDGenerator
    {
    public:
        typedef uint32_t ID;
        static constexpr ID INVALID_ID = 0;

		IDGenerator();

		ID GenerateID();
		void UnregisterID(ID& id);
        
    
    private:
		// ID is a 32-bit uint which consists of two parts;
		// - upper 16bit: version nr
		// - lower 16bit: id counter
		typedef uint16_t NUM_PART;
		typedef uint16_t VERSION_PART;

		static constexpr NUM_PART mMaxNumRegisteredIDs = std::numeric_limits<NUM_PART>::max();

		VERSION_PART GetIDVersion(const ID id) const;


		NUM_PART mNumRegisteredIDs;
		std::queue<ID> mFreeIDs;
	};


	template <typename T>
	IDGenerator<T>::IDGenerator() : mNumRegisteredIDs(0)
	{
	}

	template <typename T>
	typename IDGenerator<T>::ID IDGenerator<T>::GenerateID()
	{
		if (!empty(mFreeIDs))
		{
			ID recycledID = mFreeIDs.front();
			mFreeIDs.pop();

			const VERSION_PART oldVersion = GetIDVersion(recycledID);
			const VERSION_PART newVersion = oldVersion + 1;
			
			return recycledID | (16 << newVersion);
		}
		else
		{
			// if we reach max number of IDs assigned at the same time
			assert(mNumRegisteredIDs < mMaxNumRegisteredIDs);

			return static_cast<ID>(++mNumRegisteredIDs);
		}
	}

	template <typename T>
	void IDGenerator<T>::UnregisterID(typename IDGenerator<T>::ID& id)
	{
		mFreeIDs.push(id);

		id = INVALID_ID;
	}


	template <typename T>
	typename IDGenerator<T>::VERSION_PART IDGenerator<T>::GetIDVersion(const ID id) const
	{
		return static_cast<ID>((id >> 16) & 0xFFFFFFFF);
	}
}
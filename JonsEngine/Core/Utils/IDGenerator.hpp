#pragma once

#include "Types.h"

#include <queue>
#include <type_traits>

namespace JonsEngine
{
    // uses BASE_TYPE as data type for IDs
	// dedicates NUM_BITS_NUM_PART number of bits to store concurrently used IDs and
	// NUM_BIT_VERSION_PART number of bits for versioning for avoiding collisions
	template <typename BASE_TYPE, uint8_t NUM_BITS_NUM_PART, uint8_t NUM_BIT_VERSION_PART>
	class IDGenerator
	{
	public:
		static_assert(sizeof(BASE_TYPE) * CHAR_BIT == NUM_BITS_NUM_PART + NUM_BIT_VERSION_PART, "Number of bits in template args does not match BASE_TYPE");
		static_assert(NUM_BITS_NUM_PART > 0 && NUM_BIT_VERSION_PART > 0, "NUM_BITS_NUM_PART and NUM_BIT_VERSION_PART must be larger than zero");
		static_assert(std::is_unsigned<BASE_TYPE>::value, "BASE_TYPE must be unsigned data type");

		typedef BASE_TYPE ID;
		static constexpr ID INVALID_ID = 0;


		IDGenerator();

		ID GenerateID();
		void FreeID(ID& id);


	private:
		static const BASE_TYPE MAX_NUM_REGISTERED_IDS = (1 << NUM_BITS_NUM_PART) - 1;

		ID MaskVersionBits(const ID id) const;
		ID MaskNumBits(const ID id) const;


		BASE_TYPE mNumRegisteredIDs;
		std::queue<ID> mFreeIDs;
	};

	typedef IDGenerator<uint32_t, 24, 8> IDGenerator32;
	typedef IDGenerator<uint64_t, 56, 8> IDGenerator64;


	template <typename BASE_TYPE, uint8_t NUM_BITS_NUM_PART, uint8_t NUM_BIT_VERSION_PART>
	IDGenerator<BASE_TYPE, NUM_BITS_NUM_PART, NUM_BIT_VERSION_PART>::IDGenerator() : mNumRegisteredIDs(0)
	{
	}

	template <typename BASE_TYPE, uint8_t NUM_BITS_NUM_PART, uint8_t NUM_BIT_VERSION_PART>
	typename IDGenerator<BASE_TYPE, NUM_BITS_NUM_PART, NUM_BIT_VERSION_PART>::ID IDGenerator<BASE_TYPE, NUM_BITS_NUM_PART, NUM_BIT_VERSION_PART>::GenerateID()
	{
		if (!empty(mFreeIDs))
		{
			ID recycledID = mFreeIDs.front();
			mFreeIDs.pop();

			// we need to pick out the old version nr and increment it separately otherwise when it overflows
			// the num_part might increment and we dont want that.
			const ID newVersionNr = MaskNumBits(recycledID) + 1;

			ID newID = MaskVersionBits(recycledID);
			newID += newVersionNr;

			return newID;
		}
		else
		{
			// if we reach max number of IDs assigned at the same time
			assert(mNumRegisteredIDs < MAX_NUM_REGISTERED_IDS);

			const ID versionNr = 1;
			ID id = mNumRegisteredIDs++ << NUM_BIT_VERSION_PART;
			id += versionNr;

			return id;
		}
	}

	template <typename BASE_TYPE, uint8_t NUM_BITS_NUM_PART, uint8_t NUM_BIT_VERSION_PART>
	void IDGenerator<BASE_TYPE, NUM_BITS_NUM_PART, NUM_BIT_VERSION_PART>::FreeID(typename IDGenerator<BASE_TYPE, NUM_BITS_NUM_PART, NUM_BIT_VERSION_PART>::ID& id)
	{
		mFreeIDs.push(id);

		id = INVALID_ID;
	}


	template <typename BASE_TYPE, uint8_t NUM_BITS_NUM_PART, uint8_t NUM_BIT_VERSION_PART>
	typename IDGenerator<BASE_TYPE, NUM_BITS_NUM_PART, NUM_BIT_VERSION_PART>::ID IDGenerator<BASE_TYPE, NUM_BITS_NUM_PART, NUM_BIT_VERSION_PART>::MaskVersionBits(const ID id) const
	{
		const ID bitmask = (static_cast<ID>((1ULL << NUM_BITS_NUM_PART) - 1ULL)) << NUM_BIT_VERSION_PART;

		return id & bitmask;
	}

	template <typename BASE_TYPE, uint8_t NUM_BITS_NUM_PART, uint8_t NUM_BIT_VERSION_PART>
	typename IDGenerator<BASE_TYPE, NUM_BITS_NUM_PART, NUM_BIT_VERSION_PART>::ID IDGenerator<BASE_TYPE, NUM_BITS_NUM_PART, NUM_BIT_VERSION_PART>::MaskNumBits(const ID id) const
	{
		const ID bitmask = static_cast<ID>((1ULL << NUM_BIT_VERSION_PART) - 1ULL);

		return id & bitmask;
	}
}
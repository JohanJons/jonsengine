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
        void FreeID(ID& id);


    private:
        // ID is a 32-bit uint which consists of two parts;
        // - upper 16bit: id counter
        // - lower 16bit: version number
        typedef uint16_t NUM_PART;
        typedef uint16_t VERSION_PART;

        static constexpr NUM_PART mMaxNumRegisteredIDs = std::numeric_limits<NUM_PART>::max();


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

            const VERSION_PART oldVersion = static_cast<VERSION_PART>(recycledID);
            const VERSION_PART newVersion = oldVersion + 1;

            ID newID = recycledID & 0xFFFF0000;
            newID += newVersion;

            return newID;
        }
        else
        {
            // if we reach max number of IDs assigned at the same time
            assert(mNumRegisteredIDs < mMaxNumRegisteredIDs);

            return static_cast<ID>(++mNumRegisteredIDs) << 16;
        }
    }

    template <typename T>
    void IDGenerator<T>::FreeID(typename IDGenerator<T>::ID& id)
    {
        mFreeIDs.push(id);

        id = INVALID_ID;
    }
}
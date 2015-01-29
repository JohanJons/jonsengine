#pragma once

#include "include/Core/Types.h"

#include <vector>
#include <assert.h>

namespace JonsEngine
{
    #define IDMAP_INDEX_MASK(id) id & 0xFFFFFFFF
    #define IDMAP_VERSION_MASK(id) (id >> 32) & 0xFFFF

    /*
     * Compact-, contigous-, stable-memory ID-based container.
     */
    template<typename T>
    class IDMap
    {
    public:
        typedef uint64_t ItemID;


        template <typename... Arguments>
        ItemID AddItem(Arguments&&... args)
        {
            if (!mFreeIndices.empty())
            {
                const uint32_t freeIndex = mFreeIndices.back();
                mFreeIndices.pop_back();

                // destruct old item
                mItems[freeIndex].mItem.~T();

                Item* item = new(&mItems[freeIndex]) Item(mItems[freeIndex].mVersion + 1, args...);

                return (freeIndex | (static_cast<uint64_t>(item->mVersion) << 32));
            }
            else
            {
                mItems.emplace_back(1, args...);

                return ((mItems.size() - 1) | (static_cast<uint64_t>(1) << 32));
            }
        }

        void MarkAsFree(const ItemID id)
        {
            const uint32_t index = IDMAP_INDEX_MASK(id);
            const uint32_t version = IDMAP_VERSION_MASK(id);

            assert(mItems[index].mVersion == version);

            mFreeIndices.push_back(index);
        }

        inline T& GetItem(const ItemID id)
        {
            const uint32_t index = IDMAP_INDEX_MASK(id);
            const uint32_t version = IDMAP_VERSION_MASK(id);

            assert(mItems[index].mVersion == version);

            return mItems[index].mItem;
        }

        inline T* TryGetItem(const ItemID id) const
        {
            const uint32_t index = IDMAP_INDEX_MASK(id);
            const uint32_t version = IDMAP_VERSION_MASK(id);

            if (mItems[index].mVersion == version)
                return &mItems[index].mItem;
            else
                return nullptr;
        }

        void Clear()
        {
            mItems.clear();
            mFreeIndices.clear();
        }


    private:
        struct Item {
            uint32_t mVersion;
            T mItem;

            template <typename... Arguments>
            Item(uint32_t version, Arguments&&... args) : mVersion(version), mItem(args...)
            {
            }
        };

        std::vector<Item> mItems;
        std::vector<uint32_t> mFreeIndices;
    };
}
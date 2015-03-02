#pragma once

#include "include/Core/Types.h"

#include <vector>
#include <assert.h>

namespace JonsEngine
{
    #define IDMAP_INDEX_MASK(id) id & 0xFFFFFFFF
    #define IDMAP_VERSION_MASK(id) (id >> 16) & 0xFFFFFFFF

    /*
     * Contigous-memory ID-based container.
     */
    template<typename T>
    class IDMap
    {
    private:
        struct Item {
            uint16_t mVersion;
            T mItem;

            template <typename... Arguments>
            Item(uint16_t version, Arguments&&... args) : mVersion(version), mItem(args...)
            {
            }
        };


    public:
        typedef uint32_t ItemID;

        class iterator {
        public:
            typedef typename Item& reference;
            typedef typename Item* pointer;

            iterator();
            iterator(const iterator&);
            ~iterator();

            iterator& operator=(const iterator&);
            bool operator==(const iterator&) const;
            bool operator!=(const iterator&) const;

            iterator& operator++();

            reference operator*() const;
            pointer operator->() const;
        };

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

                return (freeIndex | (static_cast<uint32_t>(item->mVersion) << 16));
            }
            else
            {
                mItems.emplace_back(1, args...);

                return ((mItems.size() - 1) | (static_cast<uint32_t>(1) << 16));
            }
        }

        void MarkAsFree(const ItemID id)
        {
            const uint16_t index = IDMAP_INDEX_MASK(id);
            const uint16_t version = IDMAP_VERSION_MASK(id);

            assert(mItems[index].mVersion == version);

            mFreeIndices.push_back(index);
        }

        inline T& GetItem(const ItemID id)
        {
            const uint16_t index = IDMAP_INDEX_MASK(id);
            const uint16_t version = IDMAP_VERSION_MASK(id);

            assert(mItems[index].mVersion == version);

            return mItems[index].mItem;
        }

        inline T* TryGetItem(const ItemID id) const
        {
            const uint16_t index = IDMAP_INDEX_MASK(id);
            const uint16_t version = IDMAP_VERSION_MASK(id);

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

        iterator begin()                { return mItems.begin(); }
        iterator end()                  { return mItems.end(); }
        /*iterator begin()                { return mItems.begin(); }
        const_iterator begin() const    { return mItems.begin(); }
        const_iterator cbegin() const   { return mItems.cbegin(); }
        iterator end()                  { return mItems.end(); }
        const_iterator end() const      { return mItems.end(); }
        const_iterator cend() const     { return mItems.cend(); }*/


    private:
        std::vector<Item> mItems;
        std::vector<uint16_t> mFreeIndices;
    };
}
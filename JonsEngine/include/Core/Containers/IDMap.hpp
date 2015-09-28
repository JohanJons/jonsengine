#pragma once

#include "include/Core/Containers/IDMapBase.hpp"
#include "include/Core/Types.h"

#include <vector>
#include <assert.h>

namespace JonsEngine
{
    // Internal storage item
    template <typename T>
    struct IDMapItem
    {
        ItemID mID;
        T mItem;

        template <typename... Arguments>
        IDMapItem(const ItemID id, Arguments&&... args);
    };

    // Contigous-memory ID-based container.
    template<typename T>
    class IDMap : public IDMapBase<T, IDMapItem<T>>
    {
    public:
        template <typename... Arguments>
        ItemID Insert(Arguments&&... args);
        void Erase(ItemID& id);

        T& GetItem(const ItemID id);
        const T& GetItem(const ItemID id) const;

        void Clear();

        iterator begin();
        iterator end();
    };


	//
	// IDMapItem
	//

	template <typename T>
	template <typename... Arguments>
    IDMapItem<T>::IDMapItem(const ItemID id, Arguments&&... args) : mID(id), mItem{std::forward<Arguments>(args)...}
	{
	}



	//
	// IDMap
	//

    template <typename T>
	template <typename... Arguments>
	typename IDMap<T>::ItemID IDMap<T>::Insert(Arguments&&... args)
	{
        const size_t newChildIndex = mItems.size();

		if (!mFreeIndirectionIndices.empty())
		{
            const uint16_t freeIndirectionIndex = mFreeIndirectionIndices.back();
            mFreeIndirectionIndices.pop_back();

            ItemID& indirectionID = mIndirectionLayer.at(freeIndirectionIndex);
            const uint32_t version = static_cast<uint32_t>(IDMAP_VERSION_MASK(indirectionID) + 1);
            indirectionID = newChildIndex | (version << 16);
            
            const ItemID publicID = freeIndirectionIndex | (version << 16);
            mItems.emplace_back(publicID, std::forward<Arguments>(args)...);
            
            return publicID;
		}
		else
		{
            const uint32_t version = 1;
            const size_t indirectionIndex = mItems.size();
            const ItemID indirectionID = newChildIndex | (version << 16);
            mIndirectionLayer.emplace_back(indirectionID);

            const ItemID publicID = indirectionIndex | (version << 16);
            mItems.emplace_back(publicID, std::forward<Arguments>(args)...);
            
            return publicID;
		}
	}

	template <typename T>
    void IDMap<T>::Erase(ItemID& id)
	{
        ItemIterator item = GetItemIter(id);

        // decrement indirection layer indexes
        std::for_each(item, mItems.end(), [this](Item& item) { ItemID& indirID = GetIndirectionID(item.mID); --indirID; });

        mItems.erase(item);
        
        const uint16_t indirectionIndex = IDMAP_INDEX_MASK(id);
        mFreeIndirectionIndices.push_back(indirectionIndex);
	}

	template <typename T>
	T& IDMap<T>::GetItem(const ItemID id)
	{
		return GetItemIter(id)->mItem;
	}

    template <typename T>
    const T& IDMap<T>::GetItem(const ItemID id) const
    {
        return GetItemIter(id)->mItem;
    }

	template <typename T>
	void IDMap<T>::Clear()
	{
		mItems.clear();
		mIndirectionLayer.clear();
        mFreeIndirectionIndices.clear();
	}

	template <typename T>
    typename IDMap<T>::iterator IDMap<T>::begin()
	{
		return iterator(mItems.begin());
	}

	template <typename T>
    typename IDMap<T>::iterator IDMap<T>::end()
	{
		return iterator(mItems.end());
	}
}
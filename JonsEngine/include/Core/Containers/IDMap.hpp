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
        T mItem;

        // TODO: fix without the padding...
        template <typename... Arguments>
        IDMapItem(const size_t _padding, Arguments&&... args);
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
    IDMapItem<T>::IDMapItem(const size_t _padding, Arguments&&... args) : mItem{std::forward<Arguments>(args)...}
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
        const size_t _PADDING = 0;

		if (!mFreeIndirectionIndices.empty())
		{
            const uint16_t freeIndirectionIndex = mFreeIndirectionIndices.back();
            mFreeIndirectionIndices.pop_back();

            ItemID& indirectionID = mIndirectionLayer.at(freeIndirectionIndex);
            const uint32_t version = static_cast<uint32_t>(IDMAP_VERSION_MASK(indirectionID) + 1);
            indirectionID = newChildIndex | (version << 16);
            
            mItems.emplace_back(_PADDING, std::forward<Arguments>(args)...);
            
            return freeIndirectionIndex | (version << 16);
		}
		else
		{
            const uint32_t version = 1;
            const size_t indirectionIndex = mItems.size();
            const ItemID indirectionID = newChildIndex | (version << 16);
            mIndirectionLayer.emplace_back(indirectionID);

            mItems.emplace_back(_PADDING, std::forward<Arguments>(args)...);
            
            return indirectionIndex | (version << 16);
		}
	}

	template <typename T>
	void Erase(ItemID& id)
	{
        ItemIterator item = GetItemIter(id);
        mItems.erase(item);
        
        const uint16_t indirectionIndex = IDMAP_INDEX_MASK(id);
        mFreeIndices.push_back(indirectionIndex);
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
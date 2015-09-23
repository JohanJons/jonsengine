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
        uint16_t mVersion;
        T mItem;

        template <typename... Arguments>
        IDMapItem(uint16_t version, Arguments&&... args);
    };

    // Contigous-memory ID-based container.
    template<typename T>
    class IDMap : public IDMapBase<T, IDMapItem<T>>
    {
    public:
        template <typename... Arguments>
        ItemID AddItem(Arguments&&... args);
        void MarkAsFree(const ItemID id);

        T& GetItem(const ItemID id);
        const T& GetItem(const ItemID id) const;
        T* TryGetItem(const ItemID id);
        const T* TryGetItem(const ItemID id) const;

        void Clear();

        iterator begin();
        iterator end();


    private:
        std::vector<Item> mItems;
        std::vector<uint16_t> mFreeIndices;
    };


	//
	// IDMapItem
	//
	template <typename T>
	template <typename... Arguments>
    IDMapItem<T>::IDMapItem(uint16_t version, Arguments&&... args) : mVersion(version), mItem(std::forward<Arguments>(args)...)
	{
	}



	//
	// IDMap
	//
	template <typename T>
	template <typename... Arguments>
	typename IDMap<T>::ItemID IDMap<T>::AddItem(Arguments&&... args)
	{
		if (!mFreeIndices.empty())
		{
			const uint32_t freeIndex = mFreeIndices.back();
			mFreeIndices.pop_back();

			// destruct old item
			mItems[freeIndex].mItem.~T();

			Item* item = new(&mItems[freeIndex]) Item(mItems[freeIndex].mVersion + 1, std::forward<Arguments>(args)...);

			return (freeIndex | (static_cast<uint32_t>(item->mVersion) << 16));
		}
		else
		{
			mItems.emplace_back(1, std::forward<Arguments>(args)...);

			return ((mItems.size() - 1) | (static_cast<uint32_t>(1) << 16));
		}
	}

	template <typename T>
	void IDMap<T>::MarkAsFree(const ItemID id)
	{
		const uint16_t index = IDMAP_INDEX_MASK(id);
		const uint16_t version = IDMAP_VERSION_MASK(id);

		assert(mItems[index].mVersion == version);

		mFreeIndices.push_back(index);
	}

	template <typename T>
	T& IDMap<T>::GetItem(const ItemID id)
	{
		const uint16_t index = IDMAP_INDEX_MASK(id);
		const uint16_t version = IDMAP_VERSION_MASK(id);

		assert(mItems[index].mVersion == version);

		return mItems[index].mItem;
	}

    template <typename T>
    const T& IDMap<T>::GetItem(const ItemID id) const
    {
        const uint16_t index = IDMAP_INDEX_MASK(id);
        const uint16_t version = IDMAP_VERSION_MASK(id);

        assert(mItems[index].mVersion == version);

        return mItems[index].mItem;
    }

	template <typename T>
	T* IDMap<T>::TryGetItem(const ItemID id)
	{
		const uint16_t index = IDMAP_INDEX_MASK(id);
		const uint16_t version = IDMAP_VERSION_MASK(id);

		if (mItems[index].mVersion == version)
			return &mItems[index].mItem;
		else
			return nullptr;
	}

    template <typename T>
    const T* IDMap<T>::TryGetItem(const ItemID id) const
    {
        const uint16_t index = IDMAP_INDEX_MASK(id);
        const uint16_t version = IDMAP_VERSION_MASK(id);

        if (mItems[index].mVersion == version)
            return &mItems[index].mItem;
        else
            return nullptr;
    }

	template <typename T>
	void IDMap<T>::Clear()
	{
		mItems.clear();
		mFreeIndices.clear();
	}

	template <typename T>
    typename IDMap<T>::iterator IDMap<T>::begin()
	{
		return mItems.begin();
	}

	template <typename T>
    typename IDMap<T>::iterator IDMap<T>::end()
	{
		return mItems.end();
	}
}
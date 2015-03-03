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
			Item(uint16_t version, Arguments&&... args) : mVersion(version), mItem(std::forward<Arguments>(args)...)
			{
			}
		};


	public:
		typedef uint32_t ItemID;

		class iterator {
		public:
			iterator(typename const std::vector<Item>::iterator& iter) : mIterator(iter)
			{
			}


			bool operator!=(const iterator& iterator) const
			{
				return mIterator != iterator.mIterator;
			}

			iterator& operator++()
			{
				++mIterator;

				return *this;
			}

			iterator operator++(int)
			{
				iterator old(++(*this));

				return old;
			}

			const T& operator*() const
			{
				return mIterator->mItem;
			}


		private:
			typename std::vector<Item>::iterator mIterator;
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

				Item* item = new(&mItems[freeIndex]) Item(mItems[freeIndex].mVersion + 1, std::forward<Arguments>(args)...);

				return (freeIndex | (static_cast<uint32_t>(item->mVersion) << 16));
			}
			else
			{
				mItems.emplace_back(1, std::forward<Arguments>(args)...);

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

		iterator begin()
		{
			return mItems.begin();
		}

		iterator end()
		{
			return mItems.end();
		}


	private:
		std::vector<Item> mItems;
		std::vector<uint16_t> mFreeIndices;
	};
}
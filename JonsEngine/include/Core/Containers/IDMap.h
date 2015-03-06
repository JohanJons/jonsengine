#pragma once

#include "include/Core/Types.h"

#include <vector>
#include <assert.h>

namespace JonsEngine
{
	//
	// Contigous-memory ID-based container.
	//
	template<typename T>
	class IDMap
	{
	private:
		struct Item {
			uint16_t mVersion;
			T mItem;

			template <typename... Arguments>
			Item(uint16_t version, Arguments&&... args);
		};


	public:
		typedef uint32_t ItemID;

		class iterator {
		public:
			iterator(typename const std::vector<Item>::iterator& iter);

			bool operator!=(const iterator& iterator) const;
			iterator& operator++();
			iterator operator++(int);
			const T& operator*() const;


		private:
			typename std::vector<Item>::iterator mIterator;
		};

		template <typename... Arguments>
		ItemID AddItem(Arguments&&... args);
		void MarkAsFree(const ItemID id);

		T& GetItem(const ItemID id);
		T* TryGetItem(const ItemID id) const;

		void Clear();

		iterator begin();
		iterator end();


	private:
		std::vector<Item> mItems;
		std::vector<uint16_t> mFreeIndices;
	};
}

#include "include/Core/Containers/IDMap.hpp"
#pragma once

#include "IDMap.hpp"

namespace JonsEngine
{
    template <typename T>
    class IDMapTree : protected IDMap<T>
    {
    public:
        template <typename... Arguments>
        ItemID AddNode(Arguments&&... args);
        template <typename... Arguments>
        ItemID AddNode(const ItemID parent, Arguments&&... args);
        void FreeNode(const ItemID nodeID);


    private:
        std::vector<uint32_t> mIndirectionLayer;
    };



    //
    // IDMapTree
    //
    template <typename T>
    template <typename... Arguments>
    typename IDMapTree<T>::ItemID IDMapTree<T>::AddNode(Arguments&&... args)
    {
        return mAddItem();
    }

    template <typename T>
    template <typename... Arguments>
    typename IDMapTree<T>::ItemID IDMapTree<T>::AddNode(const ItemID parent, Arguments&&... args)
    {
        return mAddItem();
    }

    template <typename T>
    void IDMapTree<T>::FreeNode(const ItemID nodeID)
    {

    }
}
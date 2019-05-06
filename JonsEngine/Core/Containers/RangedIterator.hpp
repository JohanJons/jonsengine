#pragma once

namespace JonsEngine
{
    template <class Container>
    class ConstRangedIterator
    {
    public:
        typedef typename Container::const_iterator Iterator;

        ConstRangedIterator() = default;
        ConstRangedIterator(const Container& container, const size_t beginIndex, const size_t upToIndex) : 
            mContainer( &container ),
            mBeginIndex( beginIndex ),
            mEndIndex( upToIndex )
        {
            assert(upToIndex >= beginIndex);
        }

        bool IsValid() const { return mContainer != nullptr; }

        Iterator begin() const
        {
            if ( !mContainer )
                return Iterator();

            return mContainer->cbegin() + mBeginIndex;
        }

        Iterator end() const
        {
            if ( !mContainer )
                return Iterator();

            return mContainer->cbegin() + mEndIndex;
        }


    private:
        const Container* mContainer = nullptr;
        size_t mBeginIndex = 0;
        size_t mEndIndex = 0;
    };
}
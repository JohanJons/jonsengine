#pragma once

namespace JonsEngine
{
    template <class Container>
    class RangedIterator
    {
    public:
        typedef typename Container::iterator Iterator;

        RangedIterator(Container& container, const size_t beginIndex, const size_t upToIndex) : mIterBegin(container.begin() + beginIndex), mIterEnd(container.begin() + upToIndex)
        {
            assert(upToIndex >= beginIndex);
        }

        Iterator begin()
        {
            return mIterBegin;
        }

        Iterator end()
        {
            return mIterEnd;
        }


    private:
        Iterator mIterBegin;
        Iterator mIterEnd;
    };


    template <class Container>
    class ConstRangedIterator
    {
    public:
        typedef typename Container::const_iterator Iterator;

        ConstRangedIterator(const Container& container, const size_t beginIndex, const size_t upToIndex) : mIterBegin(container.cbegin() + beginIndex), mIterEnd(container.cbegin() + upToIndex)
        {
            assert(upToIndex >= beginIndex);
        }

        const Iterator begin() const
        {
            return mIterBegin;
        }

        const Iterator end() const
        {
            return mIterEnd;
        }


    private:
        Iterator mIterBegin;
        Iterator mIterEnd;
    };
}
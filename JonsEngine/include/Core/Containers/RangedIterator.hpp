#pragma once

namespace JonsEngine
{
    // TODO: this duplication of code must be redundant

    template <class Container, class InternalIterator = typename Container::iterator>
    class RangedIterator
    {
    public:
        typedef InternalIterator Iterator;

        RangedIterator(Container& container, const size_t beginIndex, const size_t upToIndex) : mIterBegin(container.begin() + beginIndex), mIterEnd(container.begin() + upToIndex)
        {
            assert(upToIndex >= beginIndex);
        }

        RangedIterator(const Iterator& begin, const Iterator& end) : mIterBegin(begin), mIterEnd(end)
        {
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


    template <class Container, class InternalIterator = typename Container::const_iterator>
    class ConstRangedIterator
    {
    public:
        typedef InternalIterator Iterator;

        ConstRangedIterator(const Container& container, const size_t beginIndex, const size_t upToIndex) : mIterBegin(container.begin() + beginIndex), mIterEnd(container.begin() + upToIndex)
        {
            assert(upToIndex >= beginIndex);
        }

        ConstRangedIterator(const Iterator& begin, const Iterator& end) : mIterBegin(begin), mIterEnd(end)
        {
        }


        Iterator begin() const
        {
            return mIterBegin;
        }

        Iterator end() const
        {
            return mIterEnd;
        }


    private:
        Iterator mIterBegin;
        Iterator mIterEnd;
    };
}
#pragma once

namespace JonsEngine
{
    template <class Container, class InternalIterator = typename Container::iterator>
    class BaseRangedIterator
    {
    public:
        typedef InternalIterator Iterator;

        BaseRangedIterator(Container& container, const size_t beginIndex, const size_t upToIndex) : mIterBegin(container.begin() + beginIndex), mIterEnd(container.begin() + upToIndex)
        {
            assert(upToIndex >= beginIndex);
        }

        BaseRangedIterator(const Iterator& begin, const Iterator& end) : mIterBegin(begin), mIterEnd(end)
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


    template <class Container, class InternalIterator = typename Container::iterator>
    using Rangediterator = BaseRangedIterator<typename Container, typename InternalIterator>;

    template <class Container, class InternalIterator = typename Container::const_iterator>
    using ConstRangedIterator = BaseRangedIterator<typename Container, typename InternalIterator>;
}
#pragma once

namespace JonsEngine
{
    template <class Container>
    class RangedIterator
    {
    public:
        typedef typename Container::iterator Iterator;

        RangedIterator(Container& container, const size_t begin, const size_t end) : mIterBegin(container.begin() + begin), mIterEnd(container.begin() + end)
        {
            assert(end >= begin);
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

        ConstRangedIterator(const Container& container, const size_t begin, const size_t end) : mIterBegin(container.cbegin() + begin), mIterEnd(container.cbegin() + end)
        {
            assert(end >= begin);
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
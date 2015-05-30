#pragma once

namespace JonsEngine
{
    template <class Container>
    class RangedIterator
    {
    public:
        typedef typename Container::iterator Iterator;

        RangedIterator(Container& container, size_t begin, size_t end) : mIterBegin(container.begin() + begin), mIterEnd(container.begin() + end)
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
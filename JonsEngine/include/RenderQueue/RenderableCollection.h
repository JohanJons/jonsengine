#pragma once

#include <limits>
#include <vector>

namespace JonsEngine
{
    template <typename RENDERABLE>
    struct RenderableCollection
    {
        typedef std::vector<RENDERABLE> Collection;
        typename typedef Collection::size_type Index;
        constexpr static Index INVALID_INDEX = std::numeric_limits<Index>::max();
    };
}
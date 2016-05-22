#pragma once

#include <limits>
#include <vector>

namespace JonsEngine
{
    template <typename RENDERABLE>
    struct RenderableContainer
    {
        typedef std::vector<RENDERABLE> ContainerType;
        typename typedef ContainerType::size_type Index;
        static constexpr Index INVALID_INDEX = std::numeric_limits<Index>::max();
    };
}
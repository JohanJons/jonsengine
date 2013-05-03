#pragma once

#include "boost/smart_ptr.hpp"

namespace JonsEngine
{
    class IMaterial;

    /* MaterialPtr definition */
    typedef boost::shared_ptr<IMaterial> MaterialPtr;

    /* IMaterial definition */
    class IMaterial
    {
    public:
        virtual ~IMaterial() { }

        virtual void Render() = 0;
    };
}
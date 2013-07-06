#pragma once

#include "boost/smart_ptr.hpp"

namespace JonsEngine
{
    class IMesh;

    /* IMesh definition */
    typedef boost::shared_ptr<IMesh> MeshPtr;

    /* IMesh definition */
    class IMesh
    {
    public:
        virtual ~IMesh() { }

        virtual void Render() = 0;
    };
}
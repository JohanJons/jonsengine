#pragma once

#include "boost/smart_ptr.hpp"

namespace JonsEngine
{
    class IVertexBuffer;

    /* IVertexBufferPtr definition */
    typedef boost::shared_ptr<IVertexBuffer> VertexBufferPtr;

    /* IVertexBuffer definition */
    class IVertexBuffer
    {
    public:
        virtual ~IVertexBuffer() { }

        virtual void Render() = 0;
    };
}
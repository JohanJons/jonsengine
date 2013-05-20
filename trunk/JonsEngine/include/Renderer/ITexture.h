#pragma once

#include "boost/smart_ptr.hpp"

namespace JonsEngine
{
    class ITexture;

    /* TexturePtr definition */
    typedef boost::shared_ptr<ITexture> TexturePtr;

    /* ITexture definition */
    class ITexture
    {
    public:
        enum TextureFormat
        {
            RGB = 0,
            RGBA,
            UNKNOWN_FORMAT
        };

        enum TextureType
        {
            DIFFUSE = 0,
            UNKNOWN_TYPE
        };

        virtual ~ITexture() { }

    };
}
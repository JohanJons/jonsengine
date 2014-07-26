#pragma once

#include "include/Core/Types.h"

namespace JonsEngine
{
    struct ConstantBufferGBuffer
    {
        Mat4 mWVPMatrix;
        Mat4 mWorldMatrix;
        float mTextureTilingFactor;
        bool mHasDiffuseTexture;
    };
}
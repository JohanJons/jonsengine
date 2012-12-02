#pragma once

#include "include/Core/Utils/Types.h"

namespace JonsEngine
{
    /*
     * Contains various math utilities
     */


    Mat4 CreatePerspectiveMatrix(const float fov, const float aspect, const float zNear, const float zFar);
}
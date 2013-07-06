#pragma once

#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    bool CreateRectangleData(const double sizeX, const double sizeY, const double sizeZ, std::vector<Vec3>& vertexData, std::vector<Vec3>& normalData, std::vector<uint32_t>& indiceData);
}
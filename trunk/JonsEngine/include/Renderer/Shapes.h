#pragma once

#include "include/Core/Types.h"

#include <vector>

namespace JonsEngine
{
    bool CreateRectangleData(const float sizeX, const float sizeY, const float sizeZ, std::vector<float>& vertexData, std::vector<float>& normalData, std::vector<float>& texcoordData, std::vector<uint32_t>& indiceData);
}
#pragma once

#include "Core/Types.h"

#include <vector>

namespace JonsEngine
{
    void CreateRectangleData(const float sizeX, const float sizeY, const float sizeZ, std::vector<float>& vertexData, std::vector<float>& normalData, std::vector<float>& texcoordData, std::vector<uint16_t>& indiceData);
	void CreatePatchXZ(const float sizeX, const float sizeZ, std::vector<float>& vertexData, std::vector<float>& normalData, std::vector<float>& texcoordData, std::vector<uint16_t>& indiceData);
	void CreateSphereData(const float radius, const uint32_t rings, const uint32_t sectors, std::vector<float>& vertexData, std::vector<float>& normalData, std::vector<float>& texcoordData, std::vector<uint16_t>& indiceData);
}
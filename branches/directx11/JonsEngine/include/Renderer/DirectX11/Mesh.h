#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Types.h"

#include <vector>
#include <memory>

namespace JonsEngine
{
    class Logger;

    class Mesh
    {
    public:
        Mesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData, Logger& logger);
        ~Mesh();


    private:
        Logger& mLogger;
    };
}
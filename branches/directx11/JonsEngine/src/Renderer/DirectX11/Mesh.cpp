#include "include/Renderer/DirectX11/Mesh.h"

namespace JonsEngine
{
    Mesh::Mesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData, Logger& logger) : mLogger(logger)
    {

    }
    
    Mesh::~Mesh()
    {

    }
}
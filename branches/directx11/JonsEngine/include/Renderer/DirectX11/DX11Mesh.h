#pragma once

#include "include/Core/EngineDefs.h"
#include "include/Core/Types.h"

#include <d3d11.h>
#include <vector>

namespace JonsEngine
{
    class Logger;

    class DX11Mesh
    {
    public:
        DX11Mesh(ID3D11Device* device, const BYTE* shaderBytecode, const uint32_t shaderBytecodeSize, const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords,
                 const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData, Logger& logger);
        ~DX11Mesh();

        void Draw(ID3D11DeviceContext* context);
        MeshID GetMeshID() const;


    private:
        Logger& mLogger;
        MeshID mMeshID;
        uint32_t mNumIndices;
        uint32_t mVertexSize;

        ID3D11Buffer* mVertexBuffer;
        ID3D11Buffer* mIndexBuffer;
        ID3D11InputLayout* mInputLayout;
    };
}
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
        enum VertexBufferSlot
        {
            VERTEX_BUFFER_SLOT_VERTICES = 0,
            VERTEX_BUFFER_SLOT_NORMALS,
            VERTEX_BUFFER_SLOT_TANGENTS,
            VERTEX_BUFFER_SLOT_BITANGENTS,
            VERTEX_BUFFER_SLOT_TEXCOORDS,
            NUM_VERTEX_BUFFER_SLOTS
        };


        DX11Mesh(ID3D11Device* device, const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords,
                 const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData, Logger& logger);
        ~DX11Mesh();

        void Draw(ID3D11DeviceContext* context);
        MeshID GetMeshID() const;


    private:
        Logger& mLogger;
        MeshID mMeshID;
        uint32_t mNumIndices;

        ID3D11Buffer* mVertexBuffer;
        ID3D11Buffer* mNormalBuffer;
        ID3D11Buffer* mTangentBuffer;
        ID3D11Buffer* mBitangentBuffer;
        ID3D11Buffer* mTexcoordBuffer;
        ID3D11Buffer* mIndexBuffer;
    };
}
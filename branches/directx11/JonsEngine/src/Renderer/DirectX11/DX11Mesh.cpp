#include "include/Renderer/DirectX11/DX11Mesh.h"

#include "include/Renderer/DirectX11/DX11Utils.h"

namespace JonsEngine
{
    struct VERTEX { float x, y, z; };

    DX11Mesh::DX11Mesh(ID3D11Device* device, const BYTE* shaderBytecode, const uint32_t shaderBytecodeSize, const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData, Logger& logger) : mLogger(logger)
    {
        VERTEX OurVertices[] =
        {
            { 0.0f, 0.5f, 0.0f },
            { 0.45f, -0.5, 0.0f },
            { -0.45f, -0.5f, 0.0f }
        };

        D3D11_BUFFER_DESC bufferDescription;
        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = D3D11_USAGE_DEFAULT;
        bufferDescription.ByteWidth = sizeof(OurVertices);
        bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA initData;
        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = OurVertices;

        DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mVertexBuffer));

        // index buffer
        uint16_t indices[] = { 0, 1, 2 };

        ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
        bufferDescription.Usage = D3D11_USAGE_DEFAULT;
        bufferDescription.ByteWidth = sizeof(indices);
        bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;

        ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
        initData.pSysMem = indices;

        DXCALL(device->CreateBuffer(&bufferDescription, &initData, &mIndexBuffer));

        D3D11_INPUT_ELEMENT_DESC inputDescription;
        ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC));
        inputDescription.SemanticName = "POSITION";
        inputDescription.SemanticIndex = 0;
        inputDescription.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription.InputSlot = 0;
        inputDescription.AlignedByteOffset = 0;
        inputDescription.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription.InstanceDataStepRate = 0;

        DXCALL(device->CreateInputLayout(&inputDescription, 1, shaderBytecode, shaderBytecodeSize, &mInputLayout));
    }
    
    DX11Mesh::~DX11Mesh()
    {
        mVertexBuffer->Release();
        mIndexBuffer->Release();
        mInputLayout->Release();
    }
}
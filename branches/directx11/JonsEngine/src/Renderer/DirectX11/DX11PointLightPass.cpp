#include "include/Renderer/DirectX11/DX11PointLightPass.h"

#include "include/Renderer/Shapes.h"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/PointLightVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/PointLightPixel.h"

namespace JonsEngine
{
    DX11Mesh CreateSphereMesh(ID3D11Device* device)
    {
        std::vector<float> vertexData, normalData, texcoordData, tangents, bitangents;
        std::vector<uint32_t> indiceData;
        if (!CreateSphereData(1.0f, 12, 24, vertexData, normalData, texcoordData, indiceData))
        {
            JONS_LOG_ERROR(Logger::GetRendererLogger(), "Failed to create sphere for shading pass");
            throw std::runtime_error("Failed to create sphere for shading pass");
        }

        return DX11Mesh(device, vertexData, normalData, texcoordData, tangents, bitangents, indiceData);
    }


    DX11PointLightPass::DX11PointLightPass(ID3D11Device* device) : mVertexShader(nullptr), mPixelShader(nullptr), mInputLayout(nullptr), mSphereMesh(CreateSphereMesh(device)), mConstantBuffer(device)
    {
        D3D11_INPUT_ELEMENT_DESC inputDescription;
        ZeroMemory(&inputDescription, sizeof(D3D11_INPUT_ELEMENT_DESC));
        inputDescription.SemanticName = "POSITION";
        inputDescription.SemanticIndex = 0;
        inputDescription.Format = DXGI_FORMAT_R32G32B32_FLOAT;
        inputDescription.InputSlot = 0;
        inputDescription.AlignedByteOffset = 0;
        inputDescription.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        inputDescription.InstanceDataStepRate = 0;
        DXCALL(device->CreateInputLayout(&inputDescription, 1, gPointLightVertexShader, sizeof(gPointLightVertexShader), &mInputLayout));

        // rasterize for front-face culling due to light volumes
        D3D11_RASTERIZER_DESC rasterizerDesc;
        ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_FRONT;
        rasterizerDesc.FrontCounterClockwise = true;
        rasterizerDesc.DepthClipEnable = true;
        rasterizerDesc.ScissorEnable = false;
        rasterizerDesc.MultisampleEnable = false;
        rasterizerDesc.AntialiasedLineEnable = false;
        DXCALL(device->CreateRasterizerState(&rasterizerDesc, &mRSCullFront));

        //GLCALL(glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP));
        //GLCALL(glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP));
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        depthStencilDesc.StencilEnable = true;
        depthStencilDesc.StencilReadMask = 0;
        depthStencilDesc.StencilWriteMask = 0;
        depthStencilDesc.FrontFace. 
        DXCALL(device->CreateDepthStencilState(&depthStencilDesc, &mDepthStencilState));

        DXCALL(device->CreateVertexShader(gPointLightVertexShader, sizeof(gPointLightVertexShader), NULL, &mVertexShader));
        DXCALL(device->CreatePixelShader(gPointLightPixelShader, sizeof(gPointLightPixelShader), NULL, &mPixelShader));
    }

    DX11PointLightPass::~DX11PointLightPass()
    {
        mVertexShader->Release();
        mPixelShader->Release();
        mInputLayout->Release();
        mDepthStencilState->Release();
        mRSCullFront->Release();
    }


    void DX11PointLightPass::BindForShading(ID3D11DeviceContext* context)
    {
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->IASetInputLayout(mInputLayout);

        context->VSSetShader(mVertexShader, NULL, NULL);
        context->PSSetShader(mPixelShader, NULL, NULL);
    }

    void DX11PointLightPass::Render(ID3D11DeviceContext* context, const Mat4& lightWVP, const Vec4& lightColor, const Vec3& lightPosition, const float lightIntensity, const float maxDistance, uint32_t screenWidth, uint32_t screenHeight)
    {
        // preserve current rs state
        ID3D11RasterizerState* previousRasterizerState = nullptr;
        context->RSGetState(&previousRasterizerState);

        // set cullface front
        context->RSSetState(mRSCullFront);

        mConstantBuffer.SetData(PointLightCBuffer(lightWVP, lightColor, Vec4(lightPosition, 1.0), Vec2(screenWidth, screenHeight), lightIntensity, maxDistance), context, 0);

        mSphereMesh.Draw(context);

        // restore rs state
        context->RSSetState(previousRasterizerState);
    }
}
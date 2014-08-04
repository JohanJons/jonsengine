#include "include/Renderer/DirectX11/DX11PointLightPass.h"

#include "include/Renderer/Shapes.h"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/NullVertex.h"
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


    DX11PointLightPass::DX11PointLightPass(ID3D11Device* device) : mShadingVertexShader(nullptr), mStencilVertexShader(nullptr), mPixelShader(nullptr), mInputLayout(nullptr), mDSSStencilPass(nullptr), mDSSShadingPass(nullptr), mSphereMesh(CreateSphereMesh(device)),
        mNullCBuffer(device), mPointLightCBuffer(device)
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

        // rasterize with no culling for stencil pass
        ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_NONE;
        rasterizerDesc.FrontCounterClockwise = true;
        rasterizerDesc.DepthClipEnable = true;
        rasterizerDesc.ScissorEnable = false;
        rasterizerDesc.MultisampleEnable = false;
        rasterizerDesc.AntialiasedLineEnable = false;
        DXCALL(device->CreateRasterizerState(&rasterizerDesc, &mRSNoCulling));

        // DSS stencil pass
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
        depthStencilDesc.StencilEnable = true;
        depthStencilDesc.StencilReadMask = 0;
        depthStencilDesc.StencilWriteMask = 0;
        depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INVERT;
        depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INVERT;
        depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        DXCALL(device->CreateDepthStencilState(&depthStencilDesc, &mDSSStencilPass));

        // DSS shading pass
        ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        depthStencilDesc.DepthEnable = false;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
        depthStencilDesc.StencilEnable = true;
        depthStencilDesc.StencilReadMask = 0xFF;
        depthStencilDesc.StencilWriteMask = 0xFF;
        depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
        depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
        DXCALL(device->CreateDepthStencilState(&depthStencilDesc, &mDSSShadingPass));

        DXCALL(device->CreateVertexShader(gPointLightVertexShader, sizeof(gPointLightVertexShader), NULL, &mShadingVertexShader));
        DXCALL(device->CreateVertexShader(gNullVertexShader, sizeof(gNullVertexShader), NULL, &mStencilVertexShader));
        DXCALL(device->CreatePixelShader(gPointLightPixelShader, sizeof(gPointLightPixelShader), NULL, &mPixelShader));
    }

    DX11PointLightPass::~DX11PointLightPass()
    {
        mShadingVertexShader->Release();
        mStencilVertexShader->Release();
        mPixelShader->Release();
        mInputLayout->Release();
        mDSSStencilPass->Release();
        mDSSShadingPass->Release();
        mRSCullFront->Release();
        mRSNoCulling->Release();
    }


    void DX11PointLightPass::BindForShading(ID3D11DeviceContext* context)
    {
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->IASetInputLayout(mInputLayout);
    }

    void DX11PointLightPass::Render(ID3D11DeviceContext* context, const Mat4& lightWVP, const Vec4& lightColor, const Vec3& lightPosition, const float lightIntensity, const float maxDistance, uint32_t screenWidth, uint32_t screenHeight)
    {
        // preserve current rs state
        ID3D11RasterizerState* prevRasterizerState = nullptr;
        ID3D11DepthStencilState* prevDepthStencilState = nullptr;
        context->RSGetState(&prevRasterizerState);
        context->OMGetDepthStencilState(&prevDepthStencilState, 0);

        //
        // stencil pass
        //
        context->OMSetDepthStencilState(mDSSStencilPass, 0);
        context->RSSetState(mRSNoCulling);

        context->VSSetShader(mStencilVertexShader, NULL, NULL);
        context->PSSetShader(NULL, NULL, NULL);

        mNullCBuffer.SetData(NullCBuffer(lightWVP), context, 0);
        mSphereMesh.Draw(context);

        //
        // shading pass
        //
        context->OMSetDepthStencilState(mDSSShadingPass, 0);
        context->RSSetState(mRSCullFront);

        context->VSSetShader(mShadingVertexShader, NULL, NULL);
        context->PSSetShader(mPixelShader, NULL, NULL);

        mPointLightCBuffer.SetData(PointLightCBuffer(lightWVP, lightColor, Vec4(lightPosition, 1.0), Vec2(screenWidth, screenHeight), lightIntensity, maxDistance), context, 0);
        mSphereMesh.Draw(context);

        // restore state
        context->OMSetDepthStencilState(prevDepthStencilState, 0);
        context->RSSetState(prevRasterizerState);
    }
}
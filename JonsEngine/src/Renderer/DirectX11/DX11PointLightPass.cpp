#include "include/Renderer/DirectX11/DX11PointLightPass.h"

#include "include/Renderer/Shapes.h"
#include "include/Renderer/DirectX11/DX11LightAccumulationbuffer.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/PointLightPixel.h"
#include "include/Core/Utils/Math.h"
#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
    const Vec3 CUBEMAP_DIRECTION_VECTORS[DX11PointLightPass::TEXTURE_CUBE_NUM_FACES] = { Vec3(1.0f, 0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
                                                                                         Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, 0.0f, 1.0f) };

    const Vec3 CUBEMAP_UP_VECTORS[DX11PointLightPass::TEXTURE_CUBE_NUM_FACES] = { Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f),
                                                                                  Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f) };


    DX11Mesh CreateSphereMesh(ID3D11DevicePtr device)
    {
        const float radius = 1.0f;
        const uint32_t rings = 12, sectors = 24;

        std::vector<float> vertexData, normalData, texcoordData, tangents, bitangents;
        std::vector<uint16_t> indiceData;
        if (!CreateSphereData(radius, rings, sectors, vertexData, normalData, texcoordData, indiceData))
        {
            JONS_LOG_ERROR(Logger::GetRendererLogger(), "Failed to create sphere for shading pass");
            throw std::runtime_error("Failed to create sphere for shading pass");
        }

        return DX11Mesh(device, vertexData, normalData, texcoordData, tangents, bitangents, indiceData, Vec3(-radius, -radius, -radius), Vec3(radius, radius, radius));
    }


    DX11PointLightPass::DX11PointLightPass(ID3D11DevicePtr device, DX11LightAccumulationbuffer& backbuffer, DX11VertexTransformPass& vertexTransformPass, const uint32_t shadowmapSize) :
        mPixelShader(nullptr), mDSSStencilPass(nullptr), mDSSShadingPass(nullptr), mSphereMesh(CreateSphereMesh(device)), mLightAccumBuffer(backbuffer), mVertexTransformPass(vertexTransformPass),
        mShadowmap(device, shadowmapSize, TEXTURE_CUBE_NUM_FACES, true), mPointLightCBuffer(device, mPointLightCBuffer.CONSTANT_BUFFER_SLOT_PIXEL)
    {
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

        DXCALL(device->CreatePixelShader(gPointLightPixelShader, sizeof(gPointLightPixelShader), NULL, &mPixelShader));
    }

    DX11PointLightPass::~DX11PointLightPass()
    {
    }


    void DX11PointLightPass::BindForShading(ID3D11DeviceContextPtr context)
    {
        // geometry transform vertex pass for stencil/shadow pass
        mVertexTransformPass.BindForTransformPass(context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    void DX11PointLightPass::Render(ID3D11DeviceContextPtr context, const RenderQueue& renderQueue, std::vector<DX11MeshPtr>& meshes, const RenderableLighting::PointLight& pointLight, const Mat4& viewMatrix, const Mat4& invProjMatrix,
        const Vec2& screenSize, const float zFar, const float zNear)
    {
        // preserve current state
        ID3D11RasterizerStatePtr prevRasterizerState = nullptr;
        ID3D11DepthStencilStatePtr prevDSState = nullptr;
        D3D11_VIEWPORT prevViewport;
        uint32_t numViewports = 1;
        context->RSGetState(&prevRasterizerState);
        context->OMGetDepthStencilState(&prevDSState, 0);
        context->RSGetViewports(&numViewports, &prevViewport);
        const Vec4 viewLightPositonV4 = viewMatrix * Vec4(pointLight.mLightPosition, 1.0);
        const Vec3 viewLightPositonV3 = Vec3(viewLightPositonV4);

        //
        // shadow pass
        //

        // unbind any set pixel shader
        context->PSSetShader(NULL, NULL, NULL);

        context->RSSetState(mRSCullFront);
        mShadowmap.BindForDrawing(context);

        for (uint32_t face = 0; face < TEXTURE_CUBE_NUM_FACES; face++)
        {
            mShadowmap.BindDepthView(context, face);
            Mat4 lightViewMatrix = glm::lookAt(viewLightPositonV3, viewLightPositonV3 + CUBEMAP_DIRECTION_VECTORS[face], CUBEMAP_UP_VECTORS[face]);
            // TODO: precompute?
            Mat4 lightProjMatrix = PerspectiveMatrixFov(90.0f, 1.0f, Z_NEAR, Z_FAR);
            mVertexTransformPass.RenderMeshes(context, renderQueue, meshes, lightProjMatrix * lightViewMatrix * viewMatrix);
        }

        //
        // stencil pass
        //

        // restore rendering to the backbuffer
        mLightAccumBuffer.BindForLightingStage(context);

        context->OMSetDepthStencilState(mDSSStencilPass, 0);
        context->RSSetState(mRSNoCulling);

        // restore screen viewport
        context->RSSetViewports(numViewports, &prevViewport);

        mVertexTransformPass.RenderMesh(context, mSphereMesh, pointLight.mWVPMatrix);

        //
        // shading pass
        //

        context->OMSetDepthStencilState(mDSSShadingPass, 0);
        context->RSSetState(mRSCullFront);
        mShadowmap.BindForReading(context);

        // set point light pixel shader and its cbuffer
        context->PSSetShader(mPixelShader, NULL, NULL);
        mPointLightCBuffer.SetData(PointLightCBuffer(invProjMatrix, pointLight.mLightColor, viewLightPositonV4, screenSize, pointLight.mLightIntensity, pointLight.mMaxDistance, zFar, zNear), context);

        // run transform pass on sphere + point light shading pass
        mVertexTransformPass.RenderMesh(context, mSphereMesh, pointLight.mWVPMatrix);

        // restore state
        context->RSSetState(prevRasterizerState);
        context->OMSetDepthStencilState(prevDSState, 0);
    }
}

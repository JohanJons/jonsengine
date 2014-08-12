#include "include/Renderer/DirectX11/DX11PointLightPass.h"

#include "include/Renderer/Shapes.h"
#include "include/Renderer/DirectX11/DX11Backbuffer.h"
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
        std::vector<float> vertexData, normalData, texcoordData, tangents, bitangents;
        std::vector<uint32_t> indiceData;
        if (!CreateSphereData(1.0f, 12, 24, vertexData, normalData, texcoordData, indiceData))
        {
            JONS_LOG_ERROR(Logger::GetRendererLogger(), "Failed to create sphere for shading pass");
            throw std::runtime_error("Failed to create sphere for shading pass");
        }

        return DX11Mesh(device, vertexData, normalData, texcoordData, tangents, bitangents, indiceData);
    }


    DX11PointLightPass::DX11PointLightPass(ID3D11DevicePtr device, DX11Backbuffer& backbuffer, DX11VertexTransformPass& vertexTransformPass, const uint32_t shadowmapSize) :
        mPixelShader(nullptr), mDSSStencilPass(nullptr), mDSSShadingPass(nullptr), mSphereMesh(CreateSphereMesh(device)), mBackbuffer(backbuffer), mVertexTransformPass(vertexTransformPass),
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
        mVertexTransformPass.BindForTransformPass(context);
    }

    void DX11PointLightPass::Render(ID3D11DeviceContextPtr context, const RenderQueue& renderQueue, std::vector<DX11MeshPtr>& meshes, const RenderableLighting::PointLight& pointLight)
    {
        // preserve current state
        ID3D11RasterizerStatePtr prevRasterizerState = nullptr;
        D3D11_VIEWPORT prevViewport;
        uint32_t numViewports = 1;
        context->RSGetState(&prevRasterizerState);
        context->RSGetViewports(&numViewports, &prevViewport);

        //
        // shadow pass
        //

        // unbind any set pixel shader
        context->PSSetShader(NULL, NULL, NULL);

        // defaults to depth rendering/testing
        context->OMSetDepthStencilState(NULL, 0);

        context->RSSetState(mRSCullFront);
        mShadowmap.BindForDrawing(context);
        for (uint32_t face = 0; face < TEXTURE_CUBE_NUM_FACES; face++)
        {
            mShadowmap.BindDepthView(context, face);
            Mat4 lightViewMatrix = glm::lookAt(pointLight.mLightPosition, pointLight.mLightPosition + CUBEMAP_DIRECTION_VECTORS[face], CUBEMAP_UP_VECTORS[face]);
            // TODO: precompute?
            Mat4 lightProjMatrix = PerspectiveMatrixFov(90.0f, 1.0f, Z_NEAR, Z_FAR);
            mVertexTransformPass.RenderMeshes(context, renderQueue, meshes, lightProjMatrix * lightViewMatrix);
        }

        //
        // stencil pass
        //

        // restore rendering to the backbuffer
        mBackbuffer.BindForShadingStage(context);

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
        mPointLightCBuffer.SetData(PointLightCBuffer(pointLight.mLightColor, Vec4(pointLight.mLightPosition, 1.0), pointLight.mLightIntensity, pointLight.mMaxDistance), context);

        // run transform pass on sphere + point light shading pass
        mVertexTransformPass.RenderMesh(context, mSphereMesh, pointLight.mWVPMatrix);

        // restore state
        context->RSSetState(prevRasterizerState);
    }
}

#include "include/Renderer/DirectX11/DX11PointLightPass.h"

#include "include/Renderer/Shapes.h"
#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/PointLightPixel.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
    const Vec3 gCubemapDirVectors[DX11PointLightPass::POINT_LIGHT_DIR_COUNT] = { Vec3(1.0f, 0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
                                                                                 Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, 0.0f, 1.0f) };

    const Vec3 gCubemapUpVectors[DX11PointLightPass::POINT_LIGHT_DIR_COUNT] = { Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f),
                                                                                Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, 1.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f) };


    DX11Mesh CreateSphereMesh(ID3D11DevicePtr device, ID3D11DeviceContextPtr context)
    {
        const float radius = 1.0f;
        const uint32_t rings = 12, sectors = 24;

        std::vector<float> vertexData, normalData, texcoordData, tangentData;
        std::vector<uint16_t> indiceData;
        if (!CreateSphereData(radius, rings, sectors, vertexData, normalData, texcoordData, indiceData))
        {
            JONS_LOG_ERROR(Logger::GetRendererLogger(), "Failed to create sphere for shading pass");
            throw std::runtime_error("Failed to create sphere for shading pass");
        }

        return DX11Mesh(device, context, vertexData, normalData, texcoordData, tangentData, indiceData, Vec3(-radius, -radius, -radius), Vec3(radius, radius, radius));
    }


    DX11PointLightPass::DX11PointLightPass(ID3D11DevicePtr device, ID3D11DeviceContextPtr context, DX11VertexTransformPass& vertexTransformPass, const EngineSettings::ShadowResolution shadowmapRes, const uint32_t windowWidth, const uint32_t windowHeight) :
        mWindowSize(windowWidth, windowHeight),
        
        mContext(context),
        
        mPixelShader(nullptr),
        mDSSStencilPass(nullptr),
        mDSSShadingPass(nullptr),
        mSphereMesh(CreateSphereMesh(device, context)),
        mVertexTransformPass(vertexTransformPass),
        mShadowmap(device, context, shadowmapRes, DX11PointLightPass::POINT_LIGHT_DIR_COUNT, true),
        mPointLightCBuffer(device, context, mPointLightCBuffer.CONSTANT_BUFFER_SLOT_PIXEL)
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
        rasterizerDesc.CullMode = D3D11_CULL_BACK;
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

        DXCALL(device->CreatePixelShader(gPointLightPixelShader, sizeof(gPointLightPixelShader), nullptr, &mPixelShader));
    }

    DX11PointLightPass::~DX11PointLightPass()
    {
    }


    void DX11PointLightPass::Render(const RenderablePointLight& pointLight, const RenderQueue::RenderData& renderData, const Mat4& camViewMatrix, const Mat4& camViewProjMatrix, const Mat4& invCameraProjMatrix)
    {
        // preserve current state
        ID3D11RasterizerStatePtr prevRasterizerState = nullptr;
        ID3D11DepthStencilStatePtr prevDSState = nullptr;
        ID3D11RenderTargetViewPtr prevRTV = nullptr;
        ID3D11DepthStencilViewPtr prevDSV = nullptr;
        D3D11_VIEWPORT prevViewport;
        uint32_t numViewports = 1;
        mContext->RSGetState(&prevRasterizerState);
        mContext->OMGetDepthStencilState(&prevDSState, 0);
        mContext->OMGetRenderTargets(1, &prevRTV, &prevDSV);
        mContext->RSGetViewports(&numViewports, &prevViewport);
        const Vec4 viewLightPositonV4 = camViewMatrix * Vec4(pointLight.mPosition, 1.0);
        const Vec3 viewLightPositonV3 = Vec3(viewLightPositonV4);


        //
        // shadow pass
        //

        // unbind any set pixel shader
        mContext->PSSetShader(nullptr, nullptr, 0);

        mContext->RSSetState(mRSCullFront);
        mShadowmap.BindForDrawing();

        const Mat4 faceProjmatrix = PerspectiveMatrixFov(90.0f, 1.0f, Z_NEAR, pointLight.mRadius);
        for (uint32_t face = 0; face < DX11PointLightPass::POINT_LIGHT_DIR_COUNT; ++face)
        {
            const Mat4 faceViewMatrix = glm::lookAt(viewLightPositonV3, viewLightPositonV3 + gCubemapDirVectors[face], gCubemapUpVectors[face]);

			mShadowmap.BindDepthView(face);
            mVertexTransformPass.RenderMeshes(renderData, pointLight, faceProjmatrix * faceViewMatrix * camViewMatrix);
        }


        //
        // stencil pass
        //

        // used to render the mesh based on camera - scales it by radius to encompass all lit pixels
        const Mat4 lightMeshWVP = camViewProjMatrix * glm::scale(glm::translate(gIdentityMatrix, pointLight.mPosition), Vec3(pointLight.mRadius));

        // restore rendering to the light acc. buffer
        mContext->OMSetRenderTargets(1, &prevRTV.p, prevDSV);
        mContext->OMSetDepthStencilState(mDSSStencilPass, 0);
        mContext->RSSetState(mRSNoCulling);

        // restore screen viewport
        mContext->RSSetViewports(numViewports, &prevViewport);

        mVertexTransformPass.RenderStaticMesh(mSphereMesh, lightMeshWVP);


        //
        // shading pass
        //

        mContext->OMSetDepthStencilState(mDSSShadingPass, 0);
        mContext->RSSetState(mRSCullFront);
        mShadowmap.BindForReading();

        // set point light pixel shader and its cbuffer
        mContext->PSSetShader(mPixelShader, nullptr, 0);
        mPointLightCBuffer.SetData(PointLightCBuffer(invCameraProjMatrix, pointLight.mColor, viewLightPositonV4, mWindowSize, pointLight.mIntensity, Z_NEAR, pointLight.mRadius));

        // run transform pass on sphere + point light shading pass
        mVertexTransformPass.RenderStaticMesh(mSphereMesh, lightMeshWVP);

        // restore state
        mContext->RSSetState(prevRasterizerState);
        mContext->OMSetDepthStencilState(prevDSState, 0);
    }
}

#include "include/Renderer/DirectX11/DX11PointLightPass.h"

#include "include/Renderer/Shapes.h"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11BackBuffer.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/NullVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/PointLightVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/PointLightPixel.h"
#include "include/Core/Utils/Math.h"

namespace JonsEngine
{
    const uint32_t TEXTURE_CUBE_NUM_FACES = 6;

    const Vec3 CUBEMAP_DIRECTION_VECTORS[TEXTURE_CUBE_NUM_FACES] = { Vec3(1.0f, 0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
                                                                     Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f) };

    const Vec3 CUBEMAP_UP_VECTORS[TEXTURE_CUBE_NUM_FACES] = { Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f),
                                                              Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f) };


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


    DX11PointLightPass::DX11PointLightPass(ID3D11Device* device, DX11BackBuffer& backbuffer, uint32_t shadowmapSize) : mShadingVertexShader(nullptr), mNullVertexShader(nullptr), mPixelShader(nullptr),
        mInputLayout(nullptr), mDSSStencilPass(nullptr), mDSSShadingPass(nullptr), mShadowmapTexture(nullptr), mShadowmapView(nullptr), mShadowmapSRV(nullptr), mSphereMesh(CreateSphereMesh(device)),
        mBackBuffer(backbuffer), mNullCBuffer(device), mPointLightCBuffer(device)
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

        // create shadowmap texture/view/srv
        D3D11_TEXTURE2D_DESC depthBufferDesc;
        ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
        depthBufferDesc.ArraySize = 6;
        depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;     // potential issue? UNORM instead?
        depthBufferDesc.Width = shadowmapSize;
        depthBufferDesc.Height = shadowmapSize;
        depthBufferDesc.MipLevels = 1;
        depthBufferDesc.SampleDesc.Count = 1;
        depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
        DXCALL(device->CreateTexture2D(&depthBufferDesc, NULL, &mShadowmapTexture));
        DXCALL(device->CreateDepthStencilView(mShadowmapTexture, NULL, &mShadowmapView));
        DXCALL(device->CreateShaderResourceView(mShadowmapTexture, NULL, &mShadowmapSRV));

        // viewport used during shadow pass
        ZeroMemory(&mShadowPassViewport, sizeof(D3D11_VIEWPORT));
        mShadowPassViewport.TopLeftX = 0;
        mShadowPassViewport.TopLeftY = 0;
        mShadowPassViewport.Width = static_cast<float>(shadowmapSize);
        mShadowPassViewport.Height = static_cast<float>(shadowmapSize);
        mShadowPassViewport.MinDepth = 0.0f;
        mShadowPassViewport.MaxDepth = 1.0f;

        // shaders used
        DXCALL(device->CreateVertexShader(gPointLightVertexShader, sizeof(gPointLightVertexShader), NULL, &mShadingVertexShader));
        DXCALL(device->CreateVertexShader(gNullVertexShader, sizeof(gNullVertexShader), NULL, &mNullVertexShader));
        DXCALL(device->CreatePixelShader(gPointLightPixelShader, sizeof(gPointLightPixelShader), NULL, &mPixelShader));
    }

    DX11PointLightPass::~DX11PointLightPass()
    {
        mShadingVertexShader->Release();
        mNullVertexShader->Release();
        mPixelShader->Release();
        mInputLayout->Release();
        mDSSStencilPass->Release();
        mDSSShadingPass->Release();
        mRSCullFront->Release();
        mRSNoCulling->Release();
        mShadowmapTexture->Release();
        mShadowmapView->Release();
        mShadowmapSRV->Release();
    }


    void DX11PointLightPass::BindForShading(ID3D11DeviceContext* context)
    {
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->IASetInputLayout(mInputLayout);
    }

    void DX11PointLightPass::Render(ID3D11DeviceContext* context, const RenderQueue& renderQueue, const RenderableLighting::PointLight& pointLight, uint32_t screenWidth, uint32_t screenHeight)
    {
        // preserve current rs state
        ID3D11RasterizerState* prevRasterizerState = nullptr;
        ID3D11DepthStencilState* prevDepthStencilState = nullptr;
        D3D11_VIEWPORT prevViewport;
        uint32_t numViewports = 1;
        context->RSGetState(&prevRasterizerState);
        context->OMGetDepthStencilState(&prevDepthStencilState, 0);
        context->RSGetViewports(&numViewports, &prevViewport);

        context->VSSetShader(mNullVertexShader, NULL, NULL);
        context->PSSetShader(NULL, NULL, NULL);

        //
        // shadow pass
        //
        context->OMSetDepthStencilState(NULL, 0);       // defaults to depth rendering/testing
        context->RSSetState(mRSCullFront);
        context->RSSetViewports(1, &mShadowPassViewport);
        for (uint32_t face = 0; face < TEXTURE_CUBE_NUM_FACES; face++)
        {
            //mOmnidirectionalShadowmap.BindShadowmapFace(face);
            Mat4 lightViewMatrix = glm::lookAt(pointLight.mLightPosition, pointLight.mLightPosition + CUBEMAP_DIRECTION_VECTORS[face], CUBEMAP_UP_VECTORS[face]);
            Mat4 lightProjMatrix = PerspectiveMatrixFov(90.0f, 1.0f, Z_NEAR, Z_FAR);
            //GeometryDepthPass(renderQueue, lightProjMatrix * lightViewMatrix);
        }


        //
        // stencil pass
        //
        context->OMSetDepthStencilState(mDSSStencilPass, 0);
        context->RSSetState(mRSNoCulling);
        // restore screen viewport
        context->RSSetViewports(1, &prevViewport);

        mNullCBuffer.SetData(NullCBuffer(pointLight.mWVPMatrix), context, 0);
        mSphereMesh.Draw(context);

        //
        // shading pass
        //
        context->OMSetDepthStencilState(mDSSShadingPass, 0);
        context->RSSetState(mRSCullFront);

        context->VSSetShader(mShadingVertexShader, NULL, NULL);
        context->PSSetShader(mPixelShader, NULL, NULL);

        mPointLightCBuffer.SetData(PointLightCBuffer(pointLight.mWVPMatrix, pointLight.mLightColor, Vec4(pointLight.mLightPosition, 1.0), Vec2(screenWidth, screenHeight), pointLight.mLightIntensity, pointLight.mMaxDistance), context, 0);
        mSphereMesh.Draw(context);

        // restore state
        context->OMSetDepthStencilState(prevDepthStencilState, 0);
        context->RSSetState(prevRasterizerState);
    }
}
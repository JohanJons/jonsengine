#include "include/Renderer/DirectX11/DX11PointLightPass.h"

#include "include/Renderer/Shapes.h"
#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11Backbuffer.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/NullVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/PointLightVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/PointLightPixel.h"
#include "include/Core/Utils/Math.h"
#include "include/Core/Logging/Logger.h"

namespace JonsEngine
{
    const Vec3 CUBEMAP_DIRECTION_VECTORS[DX11PointLightPass::TEXTURE_CUBE_NUM_FACES] = { Vec3(1.0f, 0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
                                                                                         Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f) };

    const Vec3 CUBEMAP_UP_VECTORS[DX11PointLightPass::TEXTURE_CUBE_NUM_FACES] = { Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f),
                                                                                  Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f) };

    // used to reset shadowmap from input to rendertarget
    ID3D11ShaderResourceView* const gNullSrv = nullptr;


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


    DX11PointLightPass::DX11PointLightPass(ID3D11Device* device, DX11Backbuffer& backbuffer, uint32_t shadowmapSize) : mShadingVertexShader(nullptr), mPixelShader(nullptr),
        mInputLayout(nullptr), mDSSStencilPass(nullptr), mDSSShadingPass(nullptr), mShadowmapTexture(nullptr), mShadowmapSRV(nullptr), mSphereMesh(CreateSphereMesh(device)),
        mBackbuffer(backbuffer), mNullPass(device), mPointLightCBuffer(device)
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
        depthBufferDesc.ArraySize = TEXTURE_CUBE_NUM_FACES;
        depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        depthBufferDesc.Width = shadowmapSize;
        depthBufferDesc.Height = shadowmapSize;
        depthBufferDesc.MipLevels = 1;
        depthBufferDesc.SampleDesc.Count = 1;
        depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        depthBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
        DXCALL(device->CreateTexture2D(&depthBufferDesc, NULL, &mShadowmapTexture));

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
        dsvDesc.Texture2DArray.ArraySize = 1;
        dsvDesc.Texture2DArray.MipSlice = 0;
        for (uint32_t face = 0; face < TEXTURE_CUBE_NUM_FACES; face++)
        {
            dsvDesc.Texture2DArray.FirstArraySlice = face;
            DXCALL(device->CreateDepthStencilView(mShadowmapTexture, &dsvDesc, &mShadowmapView.at(face)));
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.TextureCube.MipLevels = 1;
        srvDesc.TextureCube.MostDetailedMip = 0;
        DXCALL(device->CreateShaderResourceView(mShadowmapTexture, &srvDesc, &mShadowmapSRV));

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
        DXCALL(device->CreatePixelShader(gPointLightPixelShader, sizeof(gPointLightPixelShader), NULL, &mPixelShader));
    }

    DX11PointLightPass::~DX11PointLightPass()
    {
    }


    void DX11PointLightPass::BindForShading(ID3D11DeviceContext* context)
    {
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->IASetInputLayout(mInputLayout);
    }

    void DX11PointLightPass::Render(ID3D11DeviceContext* context, const RenderQueue& renderQueue, std::vector<DX11MeshPtr>& meshes, ID3D11DepthStencilView* gbufferDSV, const RenderableLighting::PointLight& pointLight)
    {
        // preserve current state
        ID3D11RasterizerStatePtr prevRasterizerState = nullptr;
        ID3D11DepthStencilStatePtr prevDepthStencilState = nullptr;
        D3D11_VIEWPORT prevViewport;
        uint32_t numViewports = 1;
        context->RSGetState(&prevRasterizerState);
        context->OMGetDepthStencilState(&prevDepthStencilState, 0);
        context->RSGetViewports(&numViewports, &prevViewport);

        //
        // shadow pass
        //

        // null pass for stencil/shadow pass
        mNullPass.BindForDepthStencilPass(context);

        // defaults to depth rendering/testing
        context->OMSetDepthStencilState(NULL, 0);

        context->RSSetState(mRSCullFront);
        context->RSSetViewports(1, &mShadowPassViewport);
        for (uint32_t face = 0; face < TEXTURE_CUBE_NUM_FACES; face++)
        {
            context->OMSetRenderTargets(0, NULL, mShadowmapView.at(face));
            context->ClearDepthStencilView(mShadowmapView.at(face), D3D11_CLEAR_DEPTH, 1.0f, 0);
            Mat4 lightViewMatrix = glm::lookAt(pointLight.mLightPosition, pointLight.mLightPosition + CUBEMAP_DIRECTION_VECTORS[face], CUBEMAP_UP_VECTORS[face]);
            // TODO: precompute?
            Mat4 lightProjMatrix = PerspectiveMatrixFov(90.0f, 1.0f, Z_NEAR, Z_FAR);
            mNullPass.RenderMeshes(context, renderQueue, meshes, lightProjMatrix * lightViewMatrix);
        }

        //
        // stencil pass
        //

        // restore rendering to the backbuffer
        mBackbuffer.BindForShadingStage(context, gbufferDSV);

        context->OMSetDepthStencilState(mDSSStencilPass, 0);
        context->RSSetState(mRSNoCulling);

        // restore screen viewport
        context->RSSetViewports(1, &prevViewport);

        mNullPass.RenderMesh(context, mSphereMesh, pointLight.mWVPMatrix);

        //
        // shading pass
        //
        context->OMSetDepthStencilState(mDSSShadingPass, 0);
        context->RSSetState(mRSCullFront);
        context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_DEPTH, 1, &mShadowmapSRV.p);

        context->VSSetShader(mShadingVertexShader, NULL, NULL);
        context->PSSetShader(mPixelShader, NULL, NULL);

        mPointLightCBuffer.SetData(PointLightCBuffer(pointLight.mWVPMatrix, pointLight.mLightColor, Vec4(pointLight.mLightPosition, 1.0), pointLight.mLightIntensity, pointLight.mMaxDistance), context, 0);
        mSphereMesh.Draw(context);

        // restore state
        context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_DEPTH, 1, &gNullSrv);
        context->OMSetDepthStencilState(prevDepthStencilState, 0);
        context->RSSetState(prevRasterizerState);
    }
}

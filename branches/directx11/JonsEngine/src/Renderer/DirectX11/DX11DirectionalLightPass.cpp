#include "include/Renderer/DirectX11/DX11DirectionalLightPass.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11Backbuffer.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/FullscreenTriangleVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/DirectionalLightPixel.h"
#include "include/Core/Utils/Math.h"

#include <array>

namespace JonsEngine
{
    typedef std::array<Vec4, 8> CameraFrustrum;

    const Mat4 gBiasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.5f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.5f, 0.0f,
                           0.5f, 0.5f, 0.5f, 1.0f);

    void CalculateShadowmapCascades(std::array<float, DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES>& nearDistArr, std::array<float, DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES>& farDistArr, const float nearDist, const float farDist)
    {
        const float splitWeight = 0.75f;
        const float ratio = nearDist / farDist;

        nearDistArr[0] = nearDist;
        for (uint8_t index = 1; index < DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES; index++)
        {
            const float si = index / (float)DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES;

            nearDistArr[index] = splitWeight * (nearDist * powf(ratio, si)) + (1 - splitWeight) * (nearDist + (farDist - nearDist) * si);
            farDistArr[index - 1] = nearDistArr[index] * 1.005f;
        }
        farDistArr[DX11DirectionalLightPass::NUM_SHADOWMAP_CASCADES - 1] = farDist;
    }

    CameraFrustrum CalculateCameraFrustrum(const float fovDegrees, const float aspectRatio, const float minDist, const float maxDist, const Mat4& cameraViewMatrix)
    {
        CameraFrustrum ret = { Vec4(1.0f, 1.0f, -1.0f, 1.0f), Vec4(1.0f, -1.0f, -1.0f, 1.0f), Vec4(-1.0f, -1.0f, -1.0f, 1.0f), Vec4(-1.0f, 1.0f, -1.0f, 1.0f),
                               Vec4(1.0f, -1.0f, 1.0f, 1.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(-1.0f, 1.0f, 1.0f, 1.0f), Vec4(-1.0f, -1.0f, 1.0f, 1.0f), };

        const Mat4 perspectiveMatrix = PerspectiveMatrixFov(fovDegrees, aspectRatio, minDist, maxDist);
        const Mat4 invMVP = glm::inverse(perspectiveMatrix * cameraViewMatrix);

        for (Vec4& corner : ret)
        {
            corner = invMVP * corner;
            corner /= corner.w;
        }

        return ret;
    }

    Mat4 CreateDirLightVPMatrix(const CameraFrustrum& cameraFrustrum, const Vec3& lightDir)
    {
        Mat4 lightViewMatrix = glm::lookAt(Vec3(0.0f), -glm::normalize(lightDir), Vec3(0.0f, 1.0f, 0.0f));

        Vec4 transf = lightViewMatrix * cameraFrustrum[0];
        float maxZ = transf.z, minZ = transf.z;
        float maxX = transf.x, minX = transf.x;
        float maxY = transf.y, minY = transf.y;
        for (uint32_t i = 1; i < 8; i++)
        {
            transf = lightViewMatrix * cameraFrustrum[i];

            if (transf.z > maxZ) maxZ = transf.z;
            if (transf.z < minZ) minZ = transf.z;
            if (transf.x > maxX) maxX = transf.x;
            if (transf.x < minX) minX = transf.x;
            if (transf.y > maxY) maxY = transf.y;
            if (transf.y < minY) minY = transf.y;
        }

        Mat4 viewMatrix(lightViewMatrix);
        viewMatrix[3][0] = -(minX + maxX) * 0.5f;
        viewMatrix[3][1] = -(minY + maxY) * 0.5f;
        viewMatrix[3][2] = -(minZ + maxZ) * 0.5f;
        viewMatrix[0][3] = 0.0f;
        viewMatrix[1][3] = 0.0f;
        viewMatrix[2][3] = 0.0f;
        viewMatrix[3][3] = 1.0f;

        Vec3 halfExtents((maxX - minX) * 0.5, (maxY - minY) * 0.5, (maxZ - minZ) * 0.5);

        return glm::ortho(-halfExtents.x, halfExtents.x, -halfExtents.y, halfExtents.y, halfExtents.z, -halfExtents.y) * viewMatrix;
    }


    DX11DirectionalLightPass::DX11DirectionalLightPass(ID3D11DevicePtr device, DX11Backbuffer& backbuffer, uint32_t shadowmapSize) : mVertexShader(nullptr), mPixelShader(nullptr), mShadowmapTexture(nullptr), mNullPass(device),
        mShadowmapSRV(nullptr), mBackbuffer(backbuffer), mConstantBuffer(device)
    {
        // create shadowmap texture/view/srv
        D3D11_TEXTURE2D_DESC depthBufferDesc;
        ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
        depthBufferDesc.ArraySize = NUM_SHADOWMAP_CASCADES;
        depthBufferDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        depthBufferDesc.Width = shadowmapSize;
        depthBufferDesc.Height = shadowmapSize;
        depthBufferDesc.MipLevels = 1;
        depthBufferDesc.SampleDesc.Count = 1;
        depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
        DXCALL(device->CreateTexture2D(&depthBufferDesc, NULL, &mShadowmapTexture));

        D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
        ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
        dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
        dsvDesc.Texture2DArray.ArraySize = 1;
        for (uint32_t face = 0; face < NUM_SHADOWMAP_CASCADES; face++)
        {
            dsvDesc.Texture2DArray.FirstArraySlice = face;
            DXCALL(device->CreateDepthStencilView(mShadowmapTexture, &dsvDesc, &mShadowmapView.at(face)));
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Texture2DArray.ArraySize = NUM_SHADOWMAP_CASCADES;
        srvDesc.Texture2DArray.MipLevels = 1;
        DXCALL(device->CreateShaderResourceView(mShadowmapTexture, &srvDesc, &mShadowmapSRV));

        // viewport used during shadow pass
        ZeroMemory(&mShadowPassViewport, sizeof(D3D11_VIEWPORT));
        mShadowPassViewport.TopLeftX = 0;
        mShadowPassViewport.TopLeftY = 0;
        mShadowPassViewport.Width = static_cast<float>(shadowmapSize);
        mShadowPassViewport.Height = static_cast<float>(shadowmapSize);
        mShadowPassViewport.MinDepth = 0.0f;
        mShadowPassViewport.MaxDepth = 1.0f;

        DXCALL(device->CreateVertexShader(gFullscreenTriangleVertexShader, sizeof(gFullscreenTriangleVertexShader), NULL, &mVertexShader));
        DXCALL(device->CreatePixelShader(gDirectionalLightPixelShader, sizeof(gDirectionalLightPixelShader), NULL, &mPixelShader));
    }

    DX11DirectionalLightPass::~DX11DirectionalLightPass()
    {
    }


    void DX11DirectionalLightPass::BindForShading(ID3D11DeviceContextPtr context)
    {
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        for (uint32_t index = 0; index < DX11Mesh::NUM_VERTEX_BUFFER_SLOTS; index++)
            context->IASetVertexBuffers(index, 0, NULL, 0, 0);
        context->IASetInputLayout(NULL);

        context->VSSetShader(mVertexShader, NULL, NULL);
        context->PSSetShader(mPixelShader, NULL, NULL);
    }

    void DX11DirectionalLightPass::Render(ID3D11DeviceContextPtr context, const RenderQueue& renderQueue, std::vector<DX11MeshPtr>& meshes, const float degreesFOV, const float aspectRatio, const Mat4& cameraViewMatrix, const Vec4& lightColor, const Vec3& lightDir)
    {
        D3D11_VIEWPORT prevViewport;
        uint32_t numViewports = 1;
        context->RSGetViewports(&numViewports, &prevViewport);

        /*
        mAccumulationBuffer.BindForDrawing();
        GLCALL(glViewport(0, 0, (GLsizei)mWindowWidth, (GLsizei)mWindowHeight));
        DirLightLightingPass(directionalLight, lightVPMatrices, lighting.mCameraViewMatrix, farDistArr, lighting.mGamma, lighting.mScreenSize, debugShadowmapSplits);
        */
        //
        // Shadow pass
        //
        std::array<float, NUM_SHADOWMAP_CASCADES> nearDistArr, farDistArr;
        std::array<Mat4, NUM_SHADOWMAP_CASCADES> lightVPMatrices;

        // TODO: precompute?
        CalculateShadowmapCascades(nearDistArr, farDistArr, Z_NEAR, Z_FAR);

       /* mNullPass.BindForDepthStencilPass(context);
        context->RSSetViewports(numViewports, &mShadowPassViewport);
        context->OMSetDepthStencilState(NULL, 0);
        
        for (uint32_t cascadeIndex = 0; cascadeIndex < NUM_SHADOWMAP_CASCADES; cascadeIndex++)
        {
            context->OMSetRenderTargets(0, NULL, mShadowmapView.at(cascadeIndex));
            context->ClearDepthStencilView(mShadowmapView.at(cascadeIndex), D3D11_CLEAR_DEPTH, 1.0f, 0);
            
            CameraFrustrum cameraFrustrum = CalculateCameraFrustrum(degreesFOV, aspectRatio, nearDistArr[cascadeIndex], farDistArr[cascadeIndex], cameraViewMatrix);
            lightVPMatrices[cascadeIndex] = CreateDirLightVPMatrix(cameraFrustrum, lightDir);
            mNullPass.RenderMeshes(context, renderQueue, meshes, lightVPMatrices[cascadeIndex]);

            lightVPMatrices[cascadeIndex] = gBiasMatrix * lightVPMatrices[cascadeIndex];
            farDistArr[cascadeIndex] = -farDistArr[cascadeIndex];
        }*/



        //
        // Shading pass
        //
        
        // restore rendering to backbuffer
        mBackbuffer.BindForShadingStage(context);

        // restore viewport
        context->RSSetViewports(numViewports, &prevViewport);
        
        context->PSSetShaderResources(DX11Texture::SHADER_TEXTURE_SLOT_DEPTH, 1, &mShadowmapSRV.p);

        mConstantBuffer.SetData(DirectionalLightCBuffer(lightColor, lightDir), context, 0);

        context->Draw(3, 0);
    }
}

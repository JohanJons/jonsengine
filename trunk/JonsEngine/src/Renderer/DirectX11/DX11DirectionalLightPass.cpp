#include "include/Renderer/DirectX11/DX11DirectionalLightPass.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Texture.h"
#include "include/Renderer/DirectX11/DX11Pipeline.h"
#include "include/Renderer/DirectX11/DX11FullscreenTrianglePass.h"
#include "include/Renderer/DirectX11/DX11VertexTransformPass.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/DirectionalLightPixel.h"
#include "include/Core/Utils/Math.h"

#include <array>

namespace JonsEngine
{
    typedef std::array<Vec4, 8> CameraFrustrum;

    const Mat4 gBiasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                           0.0f, -0.5f, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 0.0f,
                           0.5f, 0.5f, 0.0f, 1.0f);

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
        CameraFrustrum ret = { Vec4(1.0f, -1.0f, 0.0f, 1.0f), Vec4(1.0f, 1.0f, 0.0f, 1.0f), Vec4(-1.0f, 1.0f, 0.0f, 1.0f), Vec4(-1.0f, -1.0f, 0.0f, 1.0f),
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
        Mat4 lightViewMatrix = glm::lookAt(Vec3(0.0f), -glm::normalize(lightDir), Vec3(0.0f, -1.0f, 0.0f));

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

        lightViewMatrix[3][0] = -(minX + maxX) * 0.5f;
        lightViewMatrix[3][1] = -(minY + maxY) * 0.5f;
        lightViewMatrix[3][2] = -(minZ + maxZ) * 0.5f;

        Vec3 halfExtents((maxX - minX) * 0.5, (maxY - minY) * 0.5, (maxZ - minZ) * 0.5);

        return OrthographicMatrix(-halfExtents.x, halfExtents.x, halfExtents.y, -halfExtents.y, halfExtents.z, -halfExtents.z) * lightViewMatrix;
    }


    DX11DirectionalLightPass::DX11DirectionalLightPass(ID3D11DevicePtr device, DX11Pipeline& backbuffer, DX11FullscreenTrianglePass& fullscreenPass, DX11VertexTransformPass& transformPass, uint32_t shadowmapSize) :
        mPixelShader(nullptr), mRSDepthClamp(nullptr), mLightingAccBuffer(backbuffer), mFullscreenPass(fullscreenPass), mVertexTransformPass(transformPass), mShadowmap(device, shadowmapSize, NUM_SHADOWMAP_CASCADES, false),
        mDirLightCBuffer(device, mDirLightCBuffer.CONSTANT_BUFFER_SLOT_PIXEL)
    {
        DXCALL(device->CreatePixelShader(gDirectionalLightPixelShader, sizeof(gDirectionalLightPixelShader), NULL, &mPixelShader));

        // depth clamp to avoid meshes between frustrum split issues
        D3D11_RASTERIZER_DESC rasterizerDesc;
        ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_BACK;
        rasterizerDesc.FrontCounterClockwise = true;
        rasterizerDesc.DepthBiasClamp = 1.0f;
        rasterizerDesc.DepthClipEnable = false;
        rasterizerDesc.ScissorEnable = false;
        rasterizerDesc.MultisampleEnable = false;
        rasterizerDesc.AntialiasedLineEnable = false;
        DXCALL(device->CreateRasterizerState(&rasterizerDesc, &mRSDepthClamp));
    }

    DX11DirectionalLightPass::~DX11DirectionalLightPass()
    {
    }


    void DX11DirectionalLightPass::Render(ID3D11DeviceContextPtr context, const RenderQueue& renderQueue, std::vector<DX11MeshPtr>& meshes, const float degreesFOV, const float aspectRatio, const Mat4& cameraViewMatrix,
        const Mat4& invProjMatrix, const Vec4& lightColor, const Vec3& lightDir, const Vec2& screenSize, const bool drawFrustrums)
    {
        D3D11_VIEWPORT prevViewport;
        ID3D11RasterizerStatePtr prevRS;
        uint32_t numViewports = 1;
        context->RSGetViewports(&numViewports, &prevViewport);
        context->RSGetState(&prevRS);


        //
        // Shadow pass
        //

        // unbind any set pixel shader
        context->PSSetShader(NULL, NULL, NULL);

        // depth clamp to avoid issues with meshes between splits
        context->RSSetState(mRSDepthClamp);

        std::array<float, NUM_SHADOWMAP_CASCADES> nearDistArr, farDistArr;
        std::array<Mat4, NUM_SHADOWMAP_CASCADES> lightVPMatrices;

        // TODO: precompute?
        CalculateShadowmapCascades(nearDistArr, farDistArr, Z_NEAR, Z_FAR);

        mVertexTransformPass.BindForTransformPass(context, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        mShadowmap.BindForDrawing(context);
        
        for (uint32_t cascadeIndex = 0; cascadeIndex < NUM_SHADOWMAP_CASCADES; cascadeIndex++)
        {
            mShadowmap.BindDepthView(context, cascadeIndex);
            
            CameraFrustrum cameraFrustrum = CalculateCameraFrustrum(degreesFOV, aspectRatio, nearDistArr[cascadeIndex], farDistArr[cascadeIndex], cameraViewMatrix);
            lightVPMatrices[cascadeIndex] = CreateDirLightVPMatrix(cameraFrustrum, lightDir);
            mVertexTransformPass.RenderMeshes(context, renderQueue, meshes, lightVPMatrices[cascadeIndex]);

            lightVPMatrices[cascadeIndex] = gBiasMatrix * lightVPMatrices[cascadeIndex] * glm::inverse(cameraViewMatrix);
            farDistArr[cascadeIndex] = -farDistArr[cascadeIndex];
        }

        //
        // Shading pass
        //
        
        // restore rendering to backbuffer, rasterize state and viewport
        mLightingAccBuffer.BindForLightingStage(context);
        context->RSSetViewports(numViewports, &prevViewport);
        context->RSSetState(prevRS);
        
        // bind shadowmap SRV for reading
        mShadowmap.BindForReading(context);

        const Vec4 camLightDir = glm::normalize(cameraViewMatrix * Vec4(-lightDir, 0));

        // set dir light cbuffer data and pixel shader
        mDirLightCBuffer.SetData(DirectionalLightCBuffer(lightVPMatrices, invProjMatrix, farDistArr, lightColor, camLightDir, screenSize, static_cast<float>(mShadowmap.GetTextureSize())), context);
        context->PSSetShader(mPixelShader, NULL, NULL);

        // run fullscreen pass + dir light shading pass
        mFullscreenPass.Render(context);
    }
}

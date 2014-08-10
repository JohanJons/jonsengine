#include "include/Renderer/DirectX11/DX11DirectionalLightPass.h"

#include "include/Renderer/DirectX11/DX11Utils.h"
#include "include/Renderer/DirectX11/DX11Mesh.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/FullscreenTriangleVertex.h"
#include "include/Renderer/DirectX11/Shaders/Compiled/DirectionalLightPixel.h"
#include "include/Core/Utils/Math.h"

#include <array>

namespace JonsEngine
{
    typedef std::array<Vec4, 8> CameraFrustrum;

    const uint8_t gNumShadowmapCascades = 4;

    const Mat4 gBiasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.5f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.5f, 0.0f,
                           0.5f, 0.5f, 0.5f, 1.0f);

    void CalculateShadowmapCascades(std::array<float, gNumShadowmapCascades>& nearDistArr, std::array<float, gNumShadowmapCascades>& farDistArr, const float nearDist, const float farDist)
    {
        const float splitWeight = 0.75f;
        const float ratio = nearDist / farDist;

        nearDistArr[0] = nearDist;
        for (uint8_t index = 1; index < gNumShadowmapCascades; index++)
        {
            const float si = index / (float)gNumShadowmapCascades;

            nearDistArr[index] = splitWeight * (nearDist * powf(ratio, si)) + (1 - splitWeight) * (nearDist + (farDist - nearDist) * si);
            farDistArr[index - 1] = nearDistArr[index] * 1.005f;
        }
        farDistArr[gNumShadowmapCascades - 1] = farDist;
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


    DX11DirectionalLightPass::DX11DirectionalLightPass(ID3D11DevicePtr device) : mVertexShader(nullptr), mPixelShader(nullptr), mConstantBuffer(device)
    {
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

    void DX11DirectionalLightPass::Render(ID3D11DeviceContextPtr context, const Vec4& lightColor, const Vec3& lightDir)
    {

        /*
        // fill shadowmaps
        mDirectionalShadowmap.BindForDrawing();
        mNullProgram.UseProgram();
        GLCALL(glViewport(0, 0, (GLsizei)mShadowmapResolution, (GLsizei)mShadowmapResolution));
        for (uint8_t cascadeIndex = 0; cascadeIndex < gNumShadowmapCascades; cascadeIndex++)
        {
            CameraFrustrum cameraFrustrum = CalculateCameraFrustrum(nearDistArr[cascadeIndex], farDistArr[cascadeIndex], lighting.mCameraViewMatrix);

            lightVPMatrices[cascadeIndex] = CreateDirLightVPMatrix(cameraFrustrum, directionalLight.mLightDirection);       // TODO: bounding box problems?
            mDirectionalShadowmap.BindShadowmapCascade(cascadeIndex);
            GeometryDepthPass(renderQueue, lightVPMatrices[cascadeIndex]);

            lightVPMatrices[cascadeIndex] = gBiasMatrix * lightVPMatrices[cascadeIndex];
            farDistArr[cascadeIndex] = -farDistArr[cascadeIndex];
        }

        mAccumulationBuffer.BindForDrawing();
        GLCALL(glViewport(0, 0, (GLsizei)mWindowWidth, (GLsizei)mWindowHeight));
        DirLightLightingPass(directionalLight, lightVPMatrices, lighting.mCameraViewMatrix, farDistArr, lighting.mGamma, lighting.mScreenSize, debugShadowmapSplits);
        */
        //
        // Shadow pass
        //
        std::array<float, gNumShadowmapCascades> nearDistArr, farDistArr;
        std::array<Mat4, gNumShadowmapCascades> lightVPMatrices;

        CalculateShadowmapCascades(nearDistArr, farDistArr, Z_NEAR, Z_FAR);     // TODO: precompute?


        //
        // Shading pass
        //
        mConstantBuffer.SetData(DirectionalLightCBuffer(lightColor, lightDir), context, 0);

        context->Draw(3, 0);
    }
}

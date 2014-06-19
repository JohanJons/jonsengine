#include "include/Renderer/OpenGL3/OpenGLRenderer.h"

#include "include/Renderer/OpenGL3/OpenGLMesh.h"
#include "include/Renderer/OpenGL3/OpenGLTexture.h"
#include "include/Renderer/OpenGL3/Shader.h"
#include "include/Renderer/OpenGL3/Shaders/GeometryVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/GeometryFragmentShader.h"
#include "include/Renderer/OpenGL3/Shaders/NullVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/NullFragmentShader.h"
#include "include/Renderer/OpenGL3/Shaders/AmbientLightVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/AmbientLightFragmentShader.h"
#include "include/Renderer/OpenGL3/Shaders/PointLightVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/PointLightFragmentShader.h"
#include "include/Renderer/OpenGL3/Shaders/DirLightVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/DirLightFragmentShader.h"
#include "include/Renderer/OpenGL3/Shaders/DirLightFragmentShader_debug.h"
#include "include/Renderer/OpenGL3/Shaders/DepthVertexShader.h"
#include "include/Renderer/OpenGL3/Shaders/DepthFragmentShader.h"
#include "include/Renderer/OpenGL3/OpenGLUtils.h"
#include "include/Renderer/Shapes.h"
#include "include/Core/EngineSettings.h"
#include "include/Core/Logging/Logger.h"
#include "include/Core/Memory/HeapAllocator.h"
#include "include/Core/Utils/Math.h"

#include "GL/glew.h"
#include <array>
#include <exception>
#include <functional>
#include <utility>

namespace JonsEngine
{
    typedef std::array<Vec4, 8> CameraFrustrum;

    const float Z_NEAR = 0.1f;
    const float Z_FAR  = 100.0f;

    const uint32_t CUBEMAP_NUM_FACES = 6;   // posX => negX => posY => negY => posZ => negZ

    const Vec3 CUBEMAP_DIRECTION_VECTORS[CUBEMAP_NUM_FACES] = { Vec3(1.0f, 0.0f, 0.0f), Vec3(-1.0f, 0.0f, 0.0f), Vec3(0.0f, 1.0f, 0.0f),
                                                                Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, 1.0f), Vec3(0.0f, 0.0f, -1.0f) };

    const Vec3 CUBEMAP_UP_VECTORS[CUBEMAP_NUM_FACES] = { Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, 0.0f, -1.0f),
                                                         Vec3(0.0f, 0.0f, -1.0f), Vec3(0.0f, -1.0f, 0.0f), Vec3(0.0f, -1.0f, 0.0f) };

    const uint8_t gNumShadowmapCascades = 4;

    const Mat4 gBiasMatrix(0.5f, 0.0f, 0.0f, 0.0f,
                           0.0f, 0.5f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.5f, 0.0f,
                           0.5f, 0.5f, 0.5f, 1.0f);

    uint32_t ShadowQualityResolution(const EngineSettings::ShadowQuality shadowQuality)
    {
        switch (shadowQuality)
        {
            case EngineSettings::ShadowQuality::SHADOW_QUALITY_LOW:
                return 512;
            case EngineSettings::ShadowQuality::SHADOW_QUALITY_HIGH:
                return 2048;
            case EngineSettings::ShadowQuality::SHADOW_QUALITY_MEDIUM:
            default:
                return 1024;
        }
    }

    void BindTexture2D(const OpenGLTexture::TextureUnit textureUnit, const TextureID targetTexture, const std::vector<OpenGLTexturePtr>& textures, Logger& logger)
    {
        GLCALL(glActiveTexture(GL_TEXTURE0 + textureUnit));

        // TODO: sort textures
        auto texture = std::find_if(textures.begin(), textures.end(), [&](const OpenGLTexturePtr ptr) { return ptr->mTextureID == targetTexture; });
        if (texture == textures.end())
        {
            JONS_LOG_ERROR(logger, "Renderable TextureID out of range");
            throw std::runtime_error("Renderable TextureID out of range");
        }

        GLCALL(glBindTexture(GL_TEXTURE_2D, texture->get()->mTexture));
    }

    void UnbindTexture2D(const OpenGLTexture::TextureUnit textureUnit, Logger& logger)
    {
        GLCALL(glActiveTexture(GL_TEXTURE0 + textureUnit));
        GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

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

    CameraFrustrum CalculateCameraFrustrum(const float minDist, const float maxDist, const Mat4& cameraViewMatrix)
    {
        CameraFrustrum ret = { Vec4(1.0f, 1.0f, -1.0f, 1.0f), Vec4(1.0f, -1.0f, -1.0f, 1.0f), Vec4(-1.0f, -1.0f, -1.0f, 1.0f), Vec4(-1.0f, 1.0f, -1.0f, 1.0f),
                               Vec4(1.0f, -1.0f, 1.0f, 1.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(-1.0f, 1.0f, 1.0f, 1.0f), Vec4(-1.0f, -1.0f, 1.0f, 1.0f), };

        const Mat4 perspectiveMatrix = glm::perspective(70.0f, 1920.0f / (float)1080.0f, minDist, maxDist);
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

    template<class RenderableFunc>
    void DrawModels(const RenderQueue& renderQueue, Logger& logger, std::vector<OpenGLRenderer::OpenGLMeshPair>& meshes, RenderableFunc&& preDrawFunc)
    {
        // both containers are assumed to be sorted by MeshID ascending
        auto meshIterator = meshes.begin();
        for (const Renderable& renderable : renderQueue)
        {
            if (renderable.mMesh == INVALID_MESH_ID)
            {
                JONS_LOG_ERROR(logger, "Renderable MeshID is invalid");
                throw std::runtime_error("Renderable MeshID is invalid");
            }

            if (renderable.mMesh < meshIterator->first->mMeshID)
                continue;

            while (renderable.mMesh > meshIterator->first->mMeshID)
            {
                meshIterator++;
                if (meshIterator == meshes.end())
                {
                    JONS_LOG_ERROR(logger, "Renderable MeshID out of range");
                    throw std::runtime_error("Renderable MeshID out of range");
                }
            }

            preDrawFunc(renderable);

            GLCALL(glBindVertexArray(meshIterator->second));
            GLCALL(glDrawElements(GL_TRIANGLES, meshIterator->first->mIndices, GL_UNSIGNED_INT, 0));
            GLCALL(glBindVertexArray(0));
        }
    }


    OpenGLRenderer::OpenGLRenderer(const EngineSettings& engineSettings, IMemoryAllocatorPtr memoryAllocator) : OpenGLRenderer(engineSettings.mWindowWidth, engineSettings.mWindowHeight, ShadowQualityResolution(engineSettings.mShadowQuality), engineSettings.mAnisotropicFiltering, engineSettings.mMSAA, memoryAllocator)
    {
    }

    OpenGLRenderer::OpenGLRenderer(const std::vector<OpenGLMeshPtr>& meshes, const std::vector<OpenGLTexturePtr>& textures, const uint32_t windowWidth, const uint32_t windowHeight, const uint32_t shadowMapResolution, const EngineSettings::Anisotropic anisotropy, const EngineSettings::MSAA msaa, IMemoryAllocatorPtr memoryAllocator)
        : 
        OpenGLRenderer(windowWidth, windowHeight, shadowMapResolution, anisotropy, msaa, memoryAllocator)
    {
        // recreate the VAOs
        for (const OpenGLMeshPtr mesh : meshes)
            mMeshes.emplace_back(mesh, SetupVAO(*mesh));

        mTextures = textures;
    }

    OpenGLRenderer::OpenGLRenderer(const uint32_t windowWidth, const uint32_t windowHeight, const uint32_t shadowMapResolution, const EngineSettings::Anisotropic anisotropy, const EngineSettings::MSAA msaa, IMemoryAllocatorPtr memoryAllocator) :
        mMemoryAllocator(memoryAllocator), mLogger(Logger::GetRendererLogger()),
        mGeometryProgram("GeometryProgram", ShaderPtr(new Shader("GeometryVertexShader", gGeometryVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("GeometryFragmentShader", gGeometryFragmentShader, Shader::FRAGMENT_SHADER)), mLogger, "UnifGeometry"),
        mPointLightProgram("PointLightProgram", ShaderPtr(new Shader("PointLightVertexShader", gShadingVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("PointLightFragmentShader", gShadingFragmentShader, Shader::FRAGMENT_SHADER)), mLogger, "UnifPointLight"),
        mNullProgram("NullProgram", ShaderPtr(new Shader("NullVertexShader", gNullVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("NullFragmentShader", gNullFragmentShader, Shader::FRAGMENT_SHADER)), mLogger, "UnifNull"),
        mDirLightProgram("DirLightProgram", ShaderPtr(new Shader("DirLightVertexShader", gDirLightVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("DirLightFragmentShader", gDirLightFragmentShader, Shader::FRAGMENT_SHADER)), mLogger, "UnifDirLight"),
        mDirLightDebugProgram("DirLightDebugProgram", ShaderPtr(new Shader("DirLightVertexShader", gDirLightVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("DirLightFragmentDebugShader", gDirLightFragmentDebugShader, Shader::FRAGMENT_SHADER)), mLogger, "UnifDirLight"),
        mAmbientProgram("AmbientLightProgram", ShaderPtr(new Shader("AmbientLightVertexShader", gAmbientVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("AmbientLightFragmentShader", gAmbientFragmentShader, Shader::FRAGMENT_SHADER)), mLogger, "UnifAmbient"),
        mDepthProgram("DepthProgram", ShaderPtr(new Shader("DepthVertexShader", gDepthVertexShader, Shader::VERTEX_SHADER)), ShaderPtr(new Shader("DepthFragmentShader", gDepthFragmentShader, Shader::FRAGMENT_SHADER)), mLogger, "UnifDepth"),
                                                            // VS2012 bug workaround. TODO: fixed in VS2013, when boost is rdy
        //mDefaultProgram("DefaultProgram", ShaderPtr(new Shader("DefaultVertexShader", gVertexShader, Shader::VERTEX_SHADER)/*mMemoryAllocator->AllocateObject<Shader>("DefaultVertexShader", gVertexShader, Shader::VERTEX_SHADER), [this](Shader* shader) { mMemoryAllocator->DeallocateObject(shader); }*/), 
        //                                 ShaderPtr(new Shader("DefaultFragmentShader", gFragmentShader, Shader::FRAGMENT_SHADER)/*mMemoryAllocator->AllocateObject<Shader>("DefaultFragmentShader", gFragmentShader, Shader::FRAGMENT_SHADER), [this](Shader* shader) { mMemoryAllocator->DeallocateObject(shader); })*/), mLogger),
        mAccumulationBuffer(mLogger, windowWidth, windowHeight),
        mGBuffer(mMemoryAllocator->AllocateObject<GBuffer>(mLogger, windowWidth, windowHeight, mMSAA), [this](GBuffer* gbuffer) { mMemoryAllocator->DeallocateObject(gbuffer); }),
        mTextureSampler(0), mAnisotropicFiltering(anisotropy), mMSAA(msaa), mWindowWidth(windowWidth), mWindowHeight(windowHeight), mShadowmapResolution(shadowMapResolution),
        mShadingGeometry(mLogger), mDirectionalShadowmap(mLogger, shadowMapResolution, gNumShadowmapCascades), mOmnidirectionalShadowmap(mLogger, shadowMapResolution, CUBEMAP_NUM_FACES)
    {
        GLCALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));

        // face culling
	    GLCALL(glCullFace(GL_BACK));
	    GLCALL(glFrontFace(GL_CCW));

        // stencil testing
        GLCALL(glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP));
        GLCALL(glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP));

        // blend
        GLCALL(glBlendEquation(GL_FUNC_ADD));
        GLCALL(glBlendFunc(GL_ONE, GL_ONE));

        // z depth testing
        GLCALL(glDepthFunc(GL_LEQUAL));
        GLCALL(glDepthRange(0.0f, 1.0f));
        GLCALL(glClearDepth(1.0f));

        // diffuse/normal texture sampler setup
        GLCALL(glGenSamplers(1, &mTextureSampler));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_S, GL_REPEAT));
        GLCALL(glSamplerParameteri(mTextureSampler, GL_TEXTURE_WRAP_T, GL_REPEAT));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_GEOMETRY_DIFFUSE, mTextureSampler));
        GLCALL(glBindSampler(OpenGLTexture::TEXTURE_UNIT_GEOMETRY_NORMAL, mTextureSampler));
        SetAnisotropicFiltering(mAnisotropicFiltering);
    }

    OpenGLRenderer::~OpenGLRenderer()
    {
        for (const OpenGLMeshPair mesh : mMeshes)
            GLCALL(glDeleteVertexArrays(1, &mesh.second));

        GLCALL(glDeleteSamplers(1, &mTextureSampler));
    }


    MeshID OpenGLRenderer::CreateMesh(const std::vector<float>& vertexData, const std::vector<float>& normalData, const std::vector<float>& texCoords, const std::vector<float>& tangents, const std::vector<float>& bitangents, const std::vector<uint32_t>& indexData)
    {
        auto allocator = mMemoryAllocator;
        OpenGLMeshPtr meshPtr(allocator->AllocateObject<OpenGLMesh>(vertexData, normalData, texCoords, tangents, bitangents, indexData, mLogger), [=](OpenGLMesh* mesh) { allocator->DeallocateObject<OpenGLMesh>(mesh); });

        mMeshes.emplace_back(meshPtr, SetupVAO(*meshPtr));

        return mMeshes.back().first->mMeshID;
    }

    TextureID OpenGLRenderer::CreateTexture(TextureType textureType, const std::vector<uint8_t>& textureData, uint32_t textureWidth, uint32_t textureHeight, ColorFormat colorFormat)
    {
        auto allocator = mMemoryAllocator;
        mTextures.emplace_back(allocator->AllocateObject<OpenGLTexture>(textureData, textureWidth, textureHeight, colorFormat, textureType, mLogger), [=](OpenGLTexture* texture) { allocator->DeallocateObject<OpenGLTexture>(texture); });  // TODO: remove _VARIADIC_MAX 6 in MSVC12 with compatible boost
        
        return mTextures.back()->mTextureID;
    }


    void OpenGLRenderer::Render(const RenderQueue& renderQueue, const RenderableLighting& lighting, const DebugOptions::RenderingMode debugMode, const DebugOptions::RenderingFlags debugExtra)
    {
        // clear default fbo color/buffer/stencil 
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

        GeometryStage(renderQueue, lighting, debugExtra.test(DebugOptions::RENDER_FLAG_DRAW_LIGHTS));
        ShadingStage(renderQueue, lighting, debugExtra.test(DebugOptions::RENDER_FLAG_SHADOWMAP_SPLITS));

        RenderToScreen(debugMode, lighting.mScreenSize);
    }


    EngineSettings::Anisotropic OpenGLRenderer::GetAnisotropicFiltering() const
    {
        return mAnisotropicFiltering;
    }

    void OpenGLRenderer::SetAnisotropicFiltering(const EngineSettings::Anisotropic anisotropic)
    {
        mAnisotropicFiltering = anisotropic;

        GLCALL(glSamplerParameterf(mTextureSampler, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<float>(anisotropic)));
    }

    EngineSettings::MSAA OpenGLRenderer::GetMSAA() const
    {
        return mMSAA;
    }

    void OpenGLRenderer::SetMSAA(const EngineSettings::MSAA msaa)
    {
        mMSAA = msaa;
        mGBuffer.reset(mMemoryAllocator->AllocateObject<GBuffer>(mLogger, mWindowWidth, mWindowHeight, mMSAA));
    }


    std::vector<OpenGLMeshPtr> OpenGLRenderer::GetMeshes() const
    {
        std::vector<OpenGLMeshPtr> ret;
        ret.reserve(mMeshes.size());

        for (const OpenGLMeshPair meshPair : mMeshes)
            ret.push_back(meshPair.first);

        return ret;
    }

    std::vector<OpenGLTexturePtr> OpenGLRenderer::GetTextures() const
    {
        return std::vector<OpenGLTexturePtr>(mTextures);
    }


    float OpenGLRenderer::GetZNear() const
    { 
        return Z_NEAR; 
    }
    
    float OpenGLRenderer::GetZFar() const
    { 
        return Z_FAR;
    }

    uint32_t OpenGLRenderer::GetShadowmapResolution() const
    {
        return mShadowmapResolution;
    }


    void OpenGLRenderer::GeometryStage(const RenderQueue& renderQueue, const RenderableLighting& lighting, const bool debugLights)
    {
        mGBuffer->BindGeometryForDrawing();

        mGeometryProgram.UseProgram();
        GLCALL(glEnable(GL_CULL_FACE));
        GLCALL(glEnable(GL_DEPTH_TEST));
        GLCALL(glDepthMask(GL_TRUE));

        // clear GBuffer position/normal/diffuse textures and depth buffer
        GLCALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        auto preDrawRenderable = [&](const Renderable& renderable) 
                                    {
                                        const bool hasDiffuseTexture = renderable.mDiffuseTexture != INVALID_TEXTURE_ID;
                                        const bool hasNormalTexture = renderable.mNormalTexture != INVALID_TEXTURE_ID;

                                        mGeometryProgram.SetUniformData(UnifGeometry(renderable.mWVPMatrix, renderable.mWorldMatrix, hasDiffuseTexture, hasNormalTexture, renderable.mTextureTilingFactor));

                                        if (hasDiffuseTexture)
                                            BindTexture2D(OpenGLTexture::TEXTURE_UNIT_GEOMETRY_DIFFUSE, renderable.mDiffuseTexture, mTextures, mLogger);

                                        if (hasNormalTexture)
                                            BindTexture2D(OpenGLTexture::TEXTURE_UNIT_GEOMETRY_NORMAL, renderable.mNormalTexture, mTextures, mLogger);
                                    };

        DrawModels(renderQueue, mLogger, mMeshes, preDrawRenderable);

        UnbindTexture2D(OpenGLTexture::TEXTURE_UNIT_GEOMETRY_DIFFUSE, mLogger);
        UnbindTexture2D(OpenGLTexture::TEXTURE_UNIT_GEOMETRY_NORMAL, mLogger);

        // debug: draw all lights
        if (debugLights)
        {
            for (const RenderableLighting::PointLight& pointLight : lighting.mPointLights)
            {
                mGeometryProgram.SetUniformData(UnifGeometry(pointLight.mWVPMatrix, pointLight.mWorldMatrix, false, false, 1.0f));
                mShadingGeometry.DrawSphere();
            }
        }

        GLCALL(glDrawBuffer(GL_NONE));
        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
        GLCALL(glDisable(GL_DEPTH_TEST));
        GLCALL(glDisable(GL_CULL_FACE));
        GLCALL(glDepthMask(GL_FALSE));
        GLCALL(glUseProgram(0));
    }
        
    void OpenGLRenderer::ShadingStage(const RenderQueue& renderQueue, const RenderableLighting& lighting, const bool debugShadowmapSplits)
    {
        mGBuffer->BindGeometryForReading();
        mAccumulationBuffer.BindForDrawing();

        // clear final texture buffer
        GLCALL(glClear(GL_COLOR_BUFFER_BIT));

        // draw fullscreen rect for ambient light
        AmbientLightPass(lighting.mAmbientLight, lighting.mGamma, lighting.mScreenSize);

        // for directional and point lights
        GLCALL(glEnable(GL_BLEND));

        // do all directional lights
        for (const RenderableLighting::DirectionalLight& directionalLight : lighting.mDirectionalLights)
        {
            std::array<float, gNumShadowmapCascades> nearDistArr, farDistArr;
            std::array<Mat4, gNumShadowmapCascades> lightVPMatrices;

            CalculateShadowmapCascades(nearDistArr, farDistArr, Z_NEAR, Z_FAR);

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
                farDistArr[cascadeIndex]      = -farDistArr[cascadeIndex];
            }

            mAccumulationBuffer.BindForDrawing();
            GLCALL(glViewport(0, 0, (GLsizei)mWindowWidth, (GLsizei)mWindowHeight));
            DirLightLightingPass(directionalLight, lightVPMatrices, lighting.mCameraViewMatrix, farDistArr, lighting.mGamma, lighting.mScreenSize, debugShadowmapSplits);
        }
        
        // do all point lights
        // first populate the accbuffers depth buffer for stencil operations
        mAccumulationBuffer.BindNullForDrawing();
        mNullProgram.UseProgram();
        GeometryDepthPass(renderQueue, lighting.mCameraProjectionMatrix * lighting.mCameraViewMatrix);
        for (const RenderableLighting::PointLight& pointLight : lighting.mPointLights)
        {
            mOmnidirectionalShadowmap.BindForDrawing();
            GLCALL(glViewport(0, 0, (GLsizei)mShadowmapResolution, (GLsizei)mShadowmapResolution));
            for (uint32_t face = 0; face < CUBEMAP_NUM_FACES; face++)
            {
                mOmnidirectionalShadowmap.BindShadowmapFace(face);
                Mat4 lightViewMatrix = glm::lookAt(pointLight.mLightPosition, pointLight.mLightPosition + CUBEMAP_DIRECTION_VECTORS[face], CUBEMAP_UP_VECTORS[face]);
                Mat4 lightProjMatrix = glm::perspective(90.0f, 1.0f, Z_NEAR, Z_FAR);
                PointLightShadowPass(renderQueue, lightProjMatrix, lightViewMatrix);
            }

			GLCALL(glEnable(GL_STENCIL_TEST));
            GLCALL(glViewport(0, 0, (GLsizei)mWindowWidth, (GLsizei)mWindowHeight));
            // stencil pass first to elimiate fragments that dosnt need to be lit
            mAccumulationBuffer.BindNullForDrawing();
            PointLightStencilPass(pointLight);
        
            mAccumulationBuffer.BindForDrawing();
            mOmnidirectionalShadowmap.BindForReading();
            PointLightLightingPass(pointLight, lighting.mGamma, lighting.mScreenSize);
			GLCALL(glDisable(GL_STENCIL_TEST));
        }

        // reset state
        GLCALL(glDrawBuffer(GL_NONE));
        GLCALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
        GLCALL(glBindTexture(GL_TEXTURE_2D, 0));
        GLCALL(glDisable(GL_BLEND));
        GLCALL(glUseProgram(0));
    }

    void OpenGLRenderer::AmbientLightPass(const Vec4& ambientLight, const Vec4& gamma, const Vec2& screenSize)
    {
        mAmbientProgram.UseProgram();

        mAmbientProgram.SetUniformData(UnifAmbientLight(ambientLight, gamma, screenSize, mMSAA));
        mShadingGeometry.DrawRectangle();

        GLCALL(glUseProgram(0));
    }

    void OpenGLRenderer::GeometryDepthPass(const RenderQueue& renderQueue, const Mat4& lightVP)
    {
        GLCALL(glEnable(GL_CULL_FACE));
        GLCALL(glEnable(GL_DEPTH_TEST));
        GLCALL(glDepthMask(GL_TRUE));

        GLCALL(glClear(GL_DEPTH_BUFFER_BIT));

        auto preDrawRenderable = [&](const Renderable& renderable)
                                    {
                                        const Mat4 wvp = lightVP * renderable.mWorldMatrix;
                                        mNullProgram.SetUniformData(UnifNull(wvp));
                                    };

        DrawModels(renderQueue, mLogger, mMeshes, preDrawRenderable);

        GLCALL(glDisable(GL_DEPTH_TEST));
        GLCALL(glDisable(GL_CULL_FACE));
        GLCALL(glDepthMask(GL_FALSE));
    }

    void OpenGLRenderer::PointLightShadowPass(const RenderQueue& renderQueue, const Mat4& lightProjMatrix, const Mat4& lightViewMatrix)
    {
        mNullProgram.UseProgram();
        GLCALL(glCullFace(GL_FRONT));

        GeometryDepthPass(renderQueue, lightProjMatrix * lightViewMatrix);
		
        GLCALL(glCullFace(GL_BACK));
		GLCALL(glUseProgram(0));
    }

    void OpenGLRenderer::PointLightStencilPass(const RenderableLighting::PointLight& pointLight)
    {
        mNullProgram.UseProgram();
        GLCALL(glEnable(GL_DEPTH_TEST));
        GLCALL(glStencilFunc(GL_ALWAYS, 0, 0));

        GLCALL(glClear(GL_STENCIL_BUFFER_BIT));
        
        mNullProgram.SetUniformData(UnifNull(pointLight.mWVPMatrix));
        mShadingGeometry.DrawSphere();
        
        GLCALL(glDisable(GL_DEPTH_TEST));
        GLCALL(glUseProgram(0));
    }

    void OpenGLRenderer::PointLightLightingPass(const RenderableLighting::PointLight& pointLight, const Vec4& gamma, const Vec2& screenSize)
    {
        mPointLightProgram.UseProgram();
        GLCALL(glEnable(GL_CULL_FACE));
        GLCALL(glCullFace(GL_FRONT));   // cull FRONT face so the light dosnt get culled when inside the radius
        GLCALL(glStencilFunc(GL_NOTEQUAL, 0, 0xFF));

        mPointLightProgram.SetUniformData(UnifPointLight(pointLight.mWVPMatrix, pointLight.mLightColor, Vec4(pointLight.mLightPosition, 1.0f), gamma, screenSize, pointLight.mLightIntensity, pointLight.mMaxDistance, mMSAA));
        mShadingGeometry.DrawSphere();

        GLCALL(glCullFace(GL_BACK));
        GLCALL(glDisable(GL_CULL_FACE));
        GLCALL(glUseProgram(0));
    }

    void OpenGLRenderer::DirLightLightingPass(const RenderableLighting::DirectionalLight& dirLight, const std::array<Mat4, gNumShadowmapCascades>& lightMatrices, const Mat4& cameraViewMatrix, const std::array<float, gNumShadowmapCascades>& splitDistances, const Vec4& gamma, const Vec2& screenSize, const bool debugShadowmapSplits)
    {
        mDirectionalShadowmap.BindForReading();

        if (debugShadowmapSplits)
        {
            mDirLightDebugProgram.UseProgram();
            mDirLightDebugProgram.SetUniformData(UnifDirLight(lightMatrices, cameraViewMatrix, splitDistances, dirLight.mLightColor, Vec4(-dirLight.mLightDirection, 0.0f), gamma, screenSize, mMSAA));
        }
        else
        {
            mDirLightProgram.UseProgram();
            mDirLightProgram.SetUniformData(UnifDirLight(lightMatrices, cameraViewMatrix, splitDistances, dirLight.mLightColor, Vec4(-dirLight.mLightDirection, 0.0f), gamma, screenSize, mMSAA));
        }

        mShadingGeometry.DrawRectangle();

        GLCALL(glUseProgram(0));
    }

    void OpenGLRenderer::RenderToScreen(const DebugOptions::RenderingMode debugOptions, const Vec2& screenSize)
    {
        switch (debugOptions)
        {
            case DebugOptions::RENDER_MODE_FULL:
            {
                mAccumulationBuffer.BindForReading();
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            case DebugOptions::RENDER_MODE_POSITIONS:
            {
                mGBuffer->BindPositionForReading();
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            case DebugOptions::RENDER_MODE_NORMALS:
            {
                mGBuffer->BindNormalsForReading();
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            case DebugOptions::RENDER_MODE_DIFFUSE:
            {
                mGBuffer->BindDiffuseForReading();
                GLCALL(glBlitFramebuffer(0, 0, mWindowWidth, mWindowHeight, 0, 0, mWindowWidth, mWindowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));
                break;
            }
            case DebugOptions::RENDER_MODE_DEPTH:
            {
                mDepthProgram.UseProgram();
                mDepthProgram.SetUniformData(UnifDepth(screenSize, Z_NEAR, Z_FAR));
                mGBuffer->BindDepthForReading();
                mShadingGeometry.DrawRectangle();
                break;
            }
            default:
                break;
        }

        GLCALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
    }
}
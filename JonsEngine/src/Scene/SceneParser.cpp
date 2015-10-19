#include "include/Scene/SceneParser.h"

#include "include/Scene/Scene.h"
#include "include/Resources/ResourceManifest.h"
#include "include/Core/Types.h"
#include "include/Core/Math/Math.h"
#include "include/Core/Math/Intersection.h"

#include <vector>

namespace JonsEngine
{
    //
    // TODO: all this code related to culling/building renderqueue should be moved elsewhere
    //
    void AddMesh(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const Mesh& mesh, const Mat4& localWorldMatrix, const float tilingFactor, const MaterialID actorMaterial);
    void AddMesh(std::vector<RenderableMesh>& resultMeshes, const Mesh& mesh, const Mat4& localWorldMatrix);
    void AddAllMeshes(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const float tilingFactor, const MaterialID actorMaterial);
    void AddAllMeshes(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix);
    void CullMeshesFrustrum(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const Mat4& wvpMatrix, const float tilingFactor, const MaterialID actorMaterial);
    void CullMeshesSphere(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const Vec3& sphereCentre, const float sphereRadius);


    SceneParser::SceneParser(const ResourceManifest& resManifest) : mResourceManifest(resManifest)
    {
    }

    SceneParser::~SceneParser()
    {
    }


    const RenderQueue& SceneParser::ParseScene(const Scene& scene, const float windowAspectRatio, const float zNear, const float zFar)
    {
        mRenderQueue.Clear();

        const Camera& sceneCamera = scene.GetSceneCamera();
        const float cameraFov = sceneCamera.GetFOV();

        mRenderQueue.mCamera.mFOV = cameraFov;
        mRenderQueue.mCamera.mCameraPosition = sceneCamera.Position();
        mRenderQueue.mCamera.mCameraViewMatrix = sceneCamera.GetCameraTransform();
        mRenderQueue.mCamera.mCameraProjectionMatrix = PerspectiveMatrixFov(cameraFov, windowAspectRatio, zNear, zFar);
        mRenderQueue.mCamera.mCameraViewProjectionMatrix = mRenderQueue.mCamera.mCameraProjectionMatrix * mRenderQueue.mCamera.mCameraViewMatrix;

        for (const StaticActor& actor : scene.GetStaticActors())
        {
            const SceneNodeID sceneNodeID = actor.GetSceneNode();
            const ModelID modelID = actor.GetModel();

            if (sceneNodeID == INVALID_SCENE_NODE_ID || modelID == INVALID_MODEL_ID)
                continue;

            const SceneNode& sceneNode = scene.GetSceneNode(sceneNodeID);

            const Mat4& worldMatrix = sceneNode.GetWorldTransform();
            const Mat4 wvpMatrix = mRenderQueue.mCamera.mCameraViewProjectionMatrix *worldMatrix;

            const Model& model = mResourceManifest.GetModel(modelID);

            CullMeshesFrustrum(mResourceManifest, mRenderQueue.mCamera.mModels, model.GetRootNode(), worldMatrix, wvpMatrix, actor.GetMaterialTilingFactor(), actor.GetMaterial());
        }

        // point lights
        for (const PointLight& pointLight : scene.GetPointLights())
        {
            if (pointLight.GetSceneNode() == INVALID_SCENE_NODE_ID)
                continue;

            const SceneNode& sceneNode = scene.GetSceneNode(pointLight.GetSceneNode());//mSceneNodeTree.GetNode(pointLight.GetSceneNode());

            const Vec3 lightPosition = sceneNode.Position();

            mRenderQueue.mPointLights.emplace_back(pointLight.GetLightColor(), lightPosition, pointLight.GetIntensity(), pointLight.GetRadius());
            RenderablePointLight& renderablePointLight = mRenderQueue.mPointLights.back();

            //  cull meshes for each face
            for (const StaticActor& actor : scene.GetStaticActors())
            {
                const SceneNodeID sceneNodeID = actor.GetSceneNode();
                const ModelID modelID = actor.GetModel();

                if (sceneNodeID == INVALID_SCENE_NODE_ID || modelID == INVALID_MODEL_ID)
                    continue;

                const SceneNode& sceneNode = scene.GetSceneNode(sceneNodeID);

                const Mat4& actorWorldMatrix = sceneNode.GetWorldTransform();
                const Model& model = mResourceManifest.GetModel(modelID);
                CullMeshesSphere(renderablePointLight.mMeshes, model.GetRootNode(), actorWorldMatrix, lightPosition, pointLight.GetRadius());
            }
        }

        // dir lights
        for (const DirectionalLight& dirLight : scene.GetDirectionalLights())
        {
            mRenderQueue.mDirectionalLights.emplace_back(dirLight.GetLightColor(), glm::normalize(dirLight.GetLightDirection()), dirLight.GetNumCascades());
            RenderableDirLight& renderableDirLight = mRenderQueue.mDirectionalLights.back();

            for (uint32_t cascadeIndex = 0; cascadeIndex < dirLight.GetNumCascades(); ++cascadeIndex)
            {
                float nearZ = 0.0f, farZ = 0.0f;
                dirLight.GetSplitDistance(cascadeIndex, nearZ, farZ);

                auto kdopIterator = dirLight.GetBoundingVolume(cascadeIndex);
                for (const StaticActor& actor : scene.GetStaticActors())
                {
                    if (actor.GetSceneNode() == INVALID_SCENE_NODE_ID || actor.GetModel() == INVALID_MODEL_ID)
                        continue;

                    const SceneNode& sceneNode = scene.GetSceneNode(actor.GetSceneNode());//mSceneNodeTree.GetNode(actor.GetSceneNode());
                    const Model& model = mResourceManifest.GetModel(actor.GetModel());

                    const Mat4& worldMatrix = sceneNode.GetWorldTransform();
                    const Mat4 localWorldMatrix = worldMatrix;// *model.GetRootNode().mLocalTransform;
                    const AABB worldAABB = localWorldMatrix * model.GetRootNode().GetLocalAABB();

                    const auto aabbIntersection = Intersection(worldAABB, kdopIterator);
                    if (aabbIntersection == AABBIntersection::Inside || aabbIntersection == AABBIntersection::Partial)
                        AddAllMeshes(renderableDirLight.mMeshes, model.GetRootNode(), worldMatrix);
                }

                renderableDirLight.mCascadeSplits.emplace_back(nearZ, farZ, renderableDirLight.mMeshes.size());
            }
        }

        // misc
        mRenderQueue.mAmbientLight = scene.GetAmbientLight();

        const Skybox& skybox = mResourceManifest.GetSkybox(scene.GetSkybox());
        mRenderQueue.mSkyboxTextureID = skybox.GetSkyboxTexture();

        return mRenderQueue;
    }


    void AddMesh(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const Mesh& mesh, const Mat4& localWorldMatrix, const float tilingFactor, const MaterialID actorMaterial)
    {
        auto diffuseTexture = INVALID_DX11_MATERIAL_ID;
        auto normalTexture = INVALID_DX11_MATERIAL_ID;
        // TODO
        auto specularFactor = 0.02f;

        const bool actorHasMaterial = actorMaterial != INVALID_MATERIAL_ID;
        const MaterialID materialID = actorHasMaterial ? actorMaterial : mesh.GetDefaultMaterial();
        if (materialID != INVALID_MATERIAL_ID)
        {
            const Material& material = resourceManifest.GetMaterial(materialID);
            diffuseTexture = material.GetDiffuseTexture();
            normalTexture = material.GetNormalTexture();
            specularFactor = material.GetSpecularFactor();
        }

        resultMeshes.emplace_back(mesh.GetMesh(), localWorldMatrix, diffuseTexture, normalTexture, specularFactor, tilingFactor);
    }

    void AddMesh(std::vector<RenderableMesh>& resultMeshes, const Mesh& mesh, const Mat4& localWorldMatrix)
    {
        resultMeshes.emplace_back(mesh.GetMesh(), localWorldMatrix);
    }

    void AddAllMeshes(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const float tilingFactor, const MaterialID actorMaterial)
    {
        for (const Mesh& mesh : node.GetMeshes())
            AddMesh(resourceManifest, resultMeshes, mesh, worldMatrix * node.GetLocalTransform(), tilingFactor, actorMaterial);

        for (const ModelNode& child : node.GetAllChildren())
        {
            for (const Mesh& mesh : child.GetMeshes())
                AddMesh(resourceManifest, resultMeshes, mesh, worldMatrix * child.GetLocalTransform(), tilingFactor, actorMaterial);
        }
    }

    void AddAllMeshes(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix)
    {
        for (const Mesh& mesh : node.GetMeshes())
            AddMesh(resultMeshes, mesh, worldMatrix * node.GetLocalTransform());

        for (const ModelNode& child : node.GetAllChildren())
        {
            for (const Mesh& mesh : child.GetMeshes())
                AddMesh(resultMeshes, mesh, worldMatrix * child.GetLocalTransform());
        }
    }

    void CullMeshesFrustrum(const ResourceManifest& resourceManifest, std::vector<RenderableModel>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const Mat4& wvpMatrix, const float tilingFactor, const MaterialID actorMaterial)
    {
        const Mat4 localWVPMatrix = wvpMatrix * node.GetLocalTransform();// *node.GetLocalTransform();// *node.mLocalTransform;
        const Mat4 localWorldMatrix = worldMatrix * node.GetLocalTransform();// *node.mLocalTransform;
        const AABB nodeWorldAABB = /*localWorldMatrix */ node.GetLocalAABB();

        AABBIntersection nodeAABBIntersection = Intersection(nodeWorldAABB, localWVPMatrix);
        switch (nodeAABBIntersection)
        {
            // if partially inside, recursively test all meshes and child nodes
        case AABBIntersection::Partial:
        {
            AABBIntersection meshAABBIntersection(AABBIntersection::Inside);

            for (const Mesh& mesh : node.GetMeshes())
            {
                const AABB meshWorldAABB = /*localWorldMatrix */ mesh.GetLocalAABB();

                meshAABBIntersection = Intersection(meshWorldAABB, localWVPMatrix);
                if (meshAABBIntersection == AABBIntersection::Outside)
                    continue;

                if (meshAABBIntersection == AABBIntersection::Inside || meshAABBIntersection == AABBIntersection::Partial)
                    AddMesh(resourceManifest, resultMeshes, mesh, /*worldMatrix */ localWorldMatrix, tilingFactor, actorMaterial);
            }

            // each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
            for (const ModelNode& child : node.GetImmediateChildren())
                CullMeshesFrustrum(resourceManifest, resultMeshes, child, worldMatrix, wvpMatrix, tilingFactor, actorMaterial);

            break;
        }

        case AABBIntersection::Inside:
        {
            AddAllMeshes(resourceManifest, resultMeshes, node, worldMatrix, tilingFactor, actorMaterial);
            break;
        }

        case AABBIntersection::Outside:
        default:
            break;
        }
    }

    void CullMeshesSphere(std::vector<RenderableMesh>& resultMeshes, const ModelNode& node, const Mat4& worldMatrix, const Vec3& sphereCentre, const float sphereRadius)
    {
        const Mat4 localWorldMatrix = worldMatrix * node.GetLocalTransform();
        const AABB nodeWorldAABB = localWorldMatrix * node.GetLocalAABB();

        // test node frustrum
        AABBIntersection nodeAABBIntersection = Intersection(nodeWorldAABB, sphereCentre, sphereRadius);
        switch (nodeAABBIntersection)
        {
        case AABBIntersection::Partial:
        {
            AABBIntersection meshAABBIntersection(AABBIntersection::Inside);

            for (const Mesh& mesh : node.GetMeshes())
            {
                const AABB worldMeshAABB = localWorldMatrix * node.GetLocalAABB();

                meshAABBIntersection = Intersection(worldMeshAABB, sphereCentre, sphereRadius);
                if (meshAABBIntersection == AABBIntersection::Outside)
                    continue;

                if (meshAABBIntersection == AABBIntersection::Inside || meshAABBIntersection == AABBIntersection::Partial)
                    AddMesh(resultMeshes, mesh, localWorldMatrix);
            }

            // each modelnodes transform is assumed to be pre-multiplied, so pass the unmodified function params
            for (const ModelNode& child : node.GetImmediateChildren())
                CullMeshesSphere(resultMeshes, child, worldMatrix, sphereCentre, sphereRadius);

            break;
        }

        case AABBIntersection::Inside:
        {
            AddAllMeshes(resultMeshes, node, worldMatrix);
            break;
        }

        case AABBIntersection::Outside:
        default:
            break;
        }
    }
}
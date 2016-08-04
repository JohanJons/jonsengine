#include "include/Resources/ResourceManifest.h"
#include "include/Renderer/Shapes.h"
#include "include/Renderer/DirectX11/DX11Renderer.h"
#include "include/Core/Memory/HeapAllocator.h"

#include "boost/functional/hash.hpp"
#include <algorithm>
#include <sstream>
#include <functional>
#include <tuple>
#include <utility>

namespace JonsEngine
{
    template <typename PackageStruct>
    typename std::vector<PackageStruct>::const_iterator FindInContainer(const std::string& assetName, const std::vector<PackageStruct>& container);

    template <typename IDType>
    IDType IncrementID(const IDType prevID);


    ResourceManifest::ResourceManifest(DX11Renderer& renderer, HeapAllocator& memoryAllocator) :
        mMemoryAllocator(memoryAllocator),
        mRenderer(renderer),
        mNextAnimationID(0)
    {
    }
       
    ResourceManifest::~ResourceManifest()
    {
    }


    ModelID ResourceManifest::CreateRectangle(const std::string& modelName, const float sizeX, const float sizeY, const float sizeZ)
    {
        assert(sizeX > 0 && sizeY > 0 && sizeZ > 0);

        std::vector<float> vertexData, normalData, texcoordData, tangentData;
        std::vector<uint16_t> indiceData;
        if (!CreateRectangleData(sizeX, sizeY, sizeZ, vertexData, normalData, texcoordData, indiceData))
            return INVALID_MODEL_ID;

        const float halfX = sizeX / 2.0f;
        const float halfY = sizeY / 2.0f;
        const float halfZ = sizeZ / 2.0f;
        const Vec3 minBounds(-halfX, -halfY, -halfZ);
        const Vec3 maxBounds(halfX, halfY, halfZ);

        const DX11MeshID meshID = mRenderer.CreateMesh(vertexData, normalData, texcoordData, tangentData, indiceData, minBounds, maxBounds);
        assert(meshID != INVALID_DX11_MESH_ID);

        return mModels.Insert(modelName, minBounds, maxBounds, meshID);
    }

    ModelID ResourceManifest::CreateCube(const std::string& modelName, const float size)
    {
        return CreateRectangle(modelName, size, size, size);
    }

    ModelID ResourceManifest::CreateSphere(const std::string& modelName, const float radius, const uint32_t rings, const uint32_t sectors)
    {
        assert(radius > 0 && rings > 0 && sectors > 0);

        std::vector<float> vertexData, normalData, texcoordData, tangentData;
        std::vector<uint16_t> indiceData;
        if (!CreateSphereData(radius, rings, sectors, vertexData, normalData, texcoordData, indiceData))
            return INVALID_MODEL_ID;

        const Vec3 minBounds(-radius, -radius, -radius);
        const Vec3 maxBounds(radius, radius, radius);

        const DX11MeshID meshID = mRenderer.CreateMesh(vertexData, normalData, texcoordData, tangentData, indiceData, minBounds, maxBounds);
        assert(meshID != INVALID_DX11_MESH_ID);
        
        return mModels.Insert(modelName, minBounds, maxBounds, meshID);
    }

    ModelID ResourceManifest::LoadModel(const std::string& assetName, const JonsPackagePtr jonsPkg)
    {
        auto iter = FindInContainer<PackageModel>(assetName, jonsPkg->mModels);
        if (iter == jonsPkg->mModels.end())
            return INVALID_MODEL_ID;

        const PackageModel& pkgModel = *iter;

        // load animations, if any
        Model::AnimationList modelAnimations;
        for (const PackageAnimation& animation : pkgModel.mAnimations)
        {
            const AnimationID animationID = LoadAnimation(animation, pkgModel.mBoneParentMap, pkgModel.mSkeleton);
            modelAnimations.emplace(animation.mName, animationID);
        }

        ModelNode::InitDataList initDataList;
        ParseModelInitData(initDataList, jonsPkg, pkgModel);

        return mModels.Insert(pkgModel, initDataList, modelAnimations);
    }

    void ResourceManifest::DeleteModel(ModelID& modelID)
    {
        assert(modelID != INVALID_MODEL_ID);
        const Model& model = GetModel(modelID);

        // delete any animations associated with the model
        auto animations = model.GetAnimations();
        for (auto& animation : animations)
            DeleteAnimation(animation.second);

        mModels.Erase(modelID);
        modelID = INVALID_MODEL_ID;
    }

    const Model& ResourceManifest::GetModel(const ModelID modelID) const
    {
        return mModels.GetItem(modelID);
    }


    const Animation& ResourceManifest::GetAnimation(const AnimationID animationID) const
    {
        assert(animationID != INVALID_ANIMATION_ID);

        return mAnimations.at(animationID);
    }

    const Animation& ResourceManifest::GetAnimation(const ModelID modelID, const std::string& animationName) const
    {
        assert(modelID != INVALID_MODEL_ID);
        const Model& model = mModels.GetItem(modelID);
        
        const AnimationID animationID = model.GetAnimationID(animationName);
        assert(animationID != INVALID_ANIMATION_ID);

        return GetAnimation(animationID);
    }


    MaterialID ResourceManifest::LoadMaterial(const std::string& assetName, const JonsPackagePtr jonsPkg)
    {
        auto iter = FindInContainer<PackageMaterial>(assetName, jonsPkg->mMaterials);
        if (iter == jonsPkg->mMaterials.end())
            return INVALID_MATERIAL_ID;
        const PackageMaterial& pkgMaterial = *iter;

        DX11MaterialID diffuseTexture = INVALID_DX11_MATERIAL_ID;
        DX11MaterialID normalTexture  = INVALID_DX11_MATERIAL_ID;
        if (pkgMaterial.mHasDiffuseTexture)
        	diffuseTexture = mRenderer.CreateTexture(TextureType::TEXTURE_TYPE_DIFFUSE, pkgMaterial.mDiffuseTexture.mTextureData, pkgMaterial.mDiffuseTexture.mTextureWidth, pkgMaterial.mDiffuseTexture.mTextureHeight);
        if (pkgMaterial.mHasNormalTexture)
            normalTexture = mRenderer.CreateTexture(TextureType::TEXTURE_TYPE_NORMAL, pkgMaterial.mNormalTexture.mTextureData, pkgMaterial.mNormalTexture.mTextureWidth, pkgMaterial.mNormalTexture.mTextureHeight);

        // TODO: real specular factor
        const float specularFactor = 0.02f;

        // TODO: material colors
        return mMaterials.Insert(pkgMaterial.mName, diffuseTexture, normalTexture, pkgMaterial.mDiffuseColor, pkgMaterial.mAmbientColor, pkgMaterial.mSpecularColor, pkgMaterial.mEmissiveColor, specularFactor);
    }

    void ResourceManifest::DeleteMaterial(MaterialID& materialID)
    {
        assert(materialID != INVALID_MATERIAL_ID);

        mMaterials.Erase(materialID);
        materialID = INVALID_MATERIAL_ID;
    }

    const Material& ResourceManifest::GetMaterial(const MaterialID materialID) const
    {
        return mMaterials.GetItem(materialID);
    }


    SkyboxID ResourceManifest::LoadSkybox(const std::string& skyboxName, const JonsPackagePtr jonsPkg)
    {
        auto iter = FindInContainer<PackageSkybox>(skyboxName, jonsPkg->mSkyBoxes);
        if (iter == jonsPkg->mSkyBoxes.end())
            return INVALID_SKYBOX_ID;

        const PackageSkybox& pkgSkybox = *iter;
        const PackageTexture& pkgSkyboxTexture = pkgSkybox.mSkyboxTexture;

        const DX11MaterialID skyboxTextureID = mRenderer.CreateTexture(TextureType::TEXTURE_TYPE_SKYBOX, pkgSkyboxTexture.mTextureData, pkgSkyboxTexture.mTextureWidth, pkgSkyboxTexture.mTextureHeight);
        assert(skyboxTextureID != INVALID_DX11_MATERIAL_ID);

        return mSkyboxes.Insert(skyboxName, skyboxTextureID);
    }

    void ResourceManifest::DeleteSkybox(SkyboxID& skyboxID)
    {
        assert(skyboxID != INVALID_SKYBOX_ID);

        mSkyboxes.Erase(skyboxID);
        skyboxID = INVALID_SKYBOX_ID;
    }

    Skybox& ResourceManifest::GetSkybox(const SkyboxID skyboxID)
    {
        return mSkyboxes.GetItem(skyboxID);
    }

    const Skybox& ResourceManifest::GetSkybox(const SkyboxID skyboxID) const
    {
        return mSkyboxes.GetItem(skyboxID);
    }


    AnimationID ResourceManifest::LoadAnimation(const PackageAnimation& animation, const BoneParentMap& parentMap, const std::vector<PackageBone>& bones)
    {
        mNextAnimationID = IncrementID(mNextAnimationID);
        const Milliseconds animationDuration(animation.mDurationInMilliseconds);
		const auto& boneAnimations = animation.mBoneAnimations;

		ConstRangedIterator<BoneParentMap> parentIter(parentMap, animation.mFirstBone, animation.mEndBone);
		ConstRangedIterator<std::vector<PackageBone>> boneIter(bones, animation.mFirstBone, animation.mEndBone);

        mAnimations.emplace(std::piecewise_construct, std::forward_as_tuple(mNextAnimationID), std::forward_as_tuple(animation.mName, animationDuration, animation.mInverseRootMatrix, boneAnimations, parentIter, boneIter));

        return mNextAnimationID;
    }

    void ResourceManifest::DeleteAnimation(const AnimationID animationID)
    {
        mAnimations.erase(animationID);
    }


    void ResourceManifest::ParseModelInitData(ModelNode::InitDataList& initDataList, const JonsPackagePtr jongPkg, const PackageModel& model)
    {
        for (const PackageNode& node : model.mNodes)
        {
            for (const PackageMesh::MeshIndex meshIndex : node.mMeshes)
            {
                const PackageMesh& mesh = model.mMeshes.at(meshIndex);

                const DX11MeshID meshID = mRenderer.CreateMesh(mesh.mVertexData, mesh.mNormalData, mesh.mTexCoordsData, mesh.mTangentData, mesh.mBoneIndices, mesh.mBoneWeights, mesh.mIndiceData, mesh.mAABB.mMinBounds, mesh.mAABB.mMaxBounds);
                DX11MaterialID materialID = INVALID_DX11_MATERIAL_ID;
                const bool meshHasMaterial = mesh.mMaterialIndex != PackageMaterial::INVALID_MATERIAL_INDEX;
                if (meshHasMaterial)
                {
                    const PackageMaterial& material = jongPkg->mMaterials.at(mesh.mMaterialIndex);
                    materialID = LoadMaterial(material.mName, jongPkg);

                    assert(materialID != INVALID_DX11_MATERIAL_ID);
                }

                initDataList.emplace_back(mesh, meshID, materialID);
            }
        }
    }


    template <typename PackageStruct>
    typename std::vector<PackageStruct>::const_iterator FindInContainer(const std::string& assetName, const std::vector<PackageStruct>& container)
    {
        const size_t hashedName = boost::hash_value(assetName);

        return std::find_if(container.begin(), container.end(), [hashedName](const PackageStruct& asset) { return boost::hash_value(asset.mName) == hashedName; });
    }

    template <typename IDType>
    IDType IncrementID(const IDType prevID)
    {
        return (prevID + 1) % INVALID_ANIMATION_ID;
    }
}
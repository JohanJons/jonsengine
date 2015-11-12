#include "include/Assimp.h"

#include "include/Core/Math/Math.h"
#include "include/Core/Math/AABB.h"
#include "include/FreeImage.h"

#include <limits>

using namespace JonsEngine;

namespace JonsAssetImporter
{
    void AddStaticAABB(PackageModel& model);
    void CheckForInvalidAABB(PackageAABB& aabb);
    uint32_t GetNodeIndex(const PackageModel& model, const std::string& nodeName);

    Mat4 aiMat4ToJonsMat4(const aiMatrix4x4& aiMat);
    Quaternion aiQuatToJonsQuat(const aiQuaternion& aiQuat);
    Vec3 aiColor3DToJonsVec3(const aiColor3D& color);
    Vec3 aiVec3ToJonsVec3(const aiVector3D& vec);


    Assimp::Assimp()
    {
    }

    Assimp::~Assimp()
    {
    }


    bool Assimp::ProcessScene(const boost::filesystem::path& modelPath, const std::string& modelName, FreeImage& freeimageImporter, JonsPackagePtr pkg)
    {
        const aiScene* scene = mImporter.ReadFile(modelPath.string(), aiProcess_FlipUVs | aiProcessPreset_TargetRealtime_MaxQuality);
        if (!scene)
        {
            Log("ERROR: Assimp parsing error: ");
            Log(mImporter.GetErrorString());
            return false;
        }

        // process materials
        // map scene material indexes to actual package material indexes
        MaterialMap materialMap;
        ProcessAssimpMaterials(scene, modelPath, materialMap, freeimageImporter, pkg);

        // process model hierarchy
        pkg->mModels.emplace_back(modelName);
        PackageModel& model = pkg->mModels.back();
        uint32_t nextNodeIndex = 0;
        model.mNodes.emplace_back();
        PackageNode& rootNode = model.mNodes.back();
        if (!ProcessAssimpNode(model, rootNode, scene, scene->mRootNode, materialMap, gIdentityMatrix, rootNode.mAABB.mMinBounds, rootNode.mAABB.mMaxBounds, nextNodeIndex, nextNodeIndex))
            return false;

        if (!ProcessAssimpAnimations(model, scene))
            return false;

        AddStaticAABB(model);

        return true;
    }

    void Assimp::ProcessAssimpMaterials(const aiScene* scene, const boost::filesystem::path& modelPath, MaterialMap& materialMap, FreeImage& freeimageImporter, JonsPackagePtr pkg)
    {
        if (!scene->HasMaterials())
            return;

        for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
        {
            const aiMaterial* material = scene->mMaterials[i];
            PackageMaterial pkgMaterial;
            aiString assimpTexturePath;

            if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &assimpTexturePath) == aiReturn_SUCCESS)
            {
                const boost::filesystem::path texturePath = std::string(assimpTexturePath.C_Str());

                std::string fullTexturePath = "";
                if (modelPath.has_parent_path())
                {
                    fullTexturePath.append(modelPath.parent_path().string());
                    fullTexturePath.append("/");
                }
                fullTexturePath.append(texturePath.string());

                freeimageImporter.ProcessTexture(pkgMaterial.mDiffuseTexture, fullTexturePath);
                pkgMaterial.mHasDiffuseTexture = true;
            }

            if ((material->GetTextureCount(aiTextureType_NORMALS) > 0 && material->GetTexture(aiTextureType_NORMALS, 0, &assimpTexturePath) == aiReturn_SUCCESS) ||
                (material->GetTextureCount(aiTextureType_HEIGHT) > 0 && material->GetTexture(aiTextureType_HEIGHT, 0, &assimpTexturePath) == aiReturn_SUCCESS))
            {
                const boost::filesystem::path texturePath = std::string(assimpTexturePath.C_Str());

                std::string fullTexturePath = "";
                if (modelPath.has_parent_path())
                {
                    fullTexturePath.append(modelPath.parent_path().string());
                    fullTexturePath.append("/");
                }
                fullTexturePath.append(texturePath.string());

                freeimageImporter.ProcessTexture(pkgMaterial.mNormalTexture, fullTexturePath);
                pkgMaterial.mHasNormalTexture = true;
            }

            aiString materialName;
            aiColor3D diffuseColor;
            aiColor3D ambientColor(0.1f);
            aiColor3D specularColor;
            aiColor3D emissiveColor;

            material->Get(AI_MATKEY_NAME, materialName);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
            //if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == aiReturn_SUCCESS)     TODO
            //    ambientColor = aiColor3D(1.0f);
            material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
            material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);

            pkgMaterial.mName = materialName.C_Str();
            pkgMaterial.mDiffuseColor = aiColor3DToJonsVec3(diffuseColor);
            pkgMaterial.mAmbientColor = aiColor3DToJonsVec3(ambientColor);
            pkgMaterial.mSpecularColor = aiColor3DToJonsVec3(specularColor);
            pkgMaterial.mEmissiveColor = aiColor3DToJonsVec3(emissiveColor);

            std::vector<PackageMaterial>::iterator iter = pkg->mMaterials.insert(pkg->mMaterials.end(), pkgMaterial);

            materialMap.insert(MaterialPair(i, std::distance(pkg->mMaterials.begin(), iter)));
        }
    }

    bool Assimp::ProcessAssimpNode(PackageModel& pkgModel, PackageNode& pkgNode, const aiScene* scene, const aiNode* node, const MaterialMap& materialMap, const Mat4& parentTransform, Vec3& parentMinBounds,
        Vec3& parentMaxBounds, const uint32_t nextNodeIndex, const uint32_t parentNodeIndex)
    {
        pkgNode.mName = node->mName.C_Str();
        pkgNode.mNodeIndex = nextNodeIndex;
        pkgNode.mParentNodeIndex = parentNodeIndex;

        const Mat4 nodeTransform = parentTransform * aiMat4ToJonsMat4(node->mTransformation);
        const uint32_t numMeshes = node->mNumMeshes;
        for (uint32_t i = 0; i < numMeshes; ++i)
        {
            pkgNode.mMeshes.emplace_back();
            if (!ProcessAssimpMesh(pkgNode.mMeshes.back(), scene->mMeshes[node->mMeshes[i]], materialMap, nodeTransform, pkgNode.mAABB.mMinBounds, pkgNode.mAABB.mMaxBounds))
                return false;
        }

        const uint32_t numChildren = node->mNumChildren;
        for (uint32_t i = 0; i < numChildren; ++i)
        {
            pkgModel.mNodes.emplace_back();
            const uint32_t childIndex = pkgModel.mNodes.size() - 1;
            if (!ProcessAssimpNode(pkgModel, pkgModel.mNodes.back(), scene, node->mChildren[i], materialMap, nodeTransform, pkgNode.mAABB.mMinBounds, pkgNode.mAABB.mMaxBounds, childIndex, nextNodeIndex))
                return false;
        }

        parentMinBounds = MinVal(parentMinBounds, pkgNode.mAABB.mMinBounds);
        parentMaxBounds = MaxVal(parentMaxBounds, pkgNode.mAABB.mMaxBounds);

        // if node has no legit AABB from either its own mesh or a childrens mesh, zero length it
        // is done after updating parent aabb bounds since the result might otherwise invalidate the parents aabb when comparing it against its other childrens
        CheckForInvalidAABB(pkgNode.mAABB);

        return true;
    }

    bool Assimp::ProcessAssimpMesh(PackageMesh& pkgMesh, const aiMesh* mesh, const MaterialMap& materialMap, const Mat4& nodeTransform, Vec3& nodeMinBounds, Vec3& nodeMaxBounds)
    {
        pkgMesh.mName = mesh->mName.C_Str();

        const uint32_t numFloatsPerTriangle = 3;
        const uint32_t numFloatsPerTexcoord = 2;
        pkgMesh.mVertexData.reserve(mesh->mNumVertices * numFloatsPerTriangle);
        pkgMesh.mNormalData.reserve(mesh->mNumVertices * numFloatsPerTriangle);
        pkgMesh.mTexCoordsData.reserve(mesh->mNumVertices * numFloatsPerTexcoord);
        // store both tangents and bitangents in same buffer
        pkgMesh.mTangentData.reserve(mesh->mNumVertices * numFloatsPerTriangle * 2);;

        // vertice, normal, texcoord, tangents and bitangents data
        for (unsigned int j = 0; j < mesh->mNumVertices; ++j)
        {
            const Vec3 transformedVertices = Vec3(nodeTransform * Vec4(aiVec3ToJonsVec3(mesh->mVertices[j]), 1.0f));
            pkgMesh.mVertexData.push_back(transformedVertices.x);
            pkgMesh.mVertexData.push_back(transformedVertices.y);
            pkgMesh.mVertexData.push_back(transformedVertices.z);

            if (mesh->HasNormals())
            {
                const Vec3 transformedNormals = Vec3(nodeTransform * Vec4(aiVec3ToJonsVec3(mesh->mNormals[j]), 0.0f));
                pkgMesh.mNormalData.push_back(transformedNormals.x);
                pkgMesh.mNormalData.push_back(transformedNormals.y);
                pkgMesh.mNormalData.push_back(transformedNormals.z);
            }

            if (mesh->HasTextureCoords(0))
            {
                pkgMesh.mTexCoordsData.push_back(mesh->mTextureCoords[0][j].x);
                pkgMesh.mTexCoordsData.push_back(mesh->mTextureCoords[0][j].y);
            }

            if (mesh->HasTangentsAndBitangents())
            {
                const Vec3 transformedTangents = Vec3(nodeTransform * Vec4(aiVec3ToJonsVec3(mesh->mTangents[j]), 0.0f));
                pkgMesh.mTangentData.push_back(transformedTangents.x);
                pkgMesh.mTangentData.push_back(transformedTangents.y);
                pkgMesh.mTangentData.push_back(transformedTangents.z);

                const Vec3 transformedBitangents = Vec3(nodeTransform * Vec4(aiVec3ToJonsVec3(mesh->mBitangents[j]), 0.0f));
                pkgMesh.mTangentData.push_back(transformedBitangents.x);
                pkgMesh.mTangentData.push_back(transformedBitangents.y);
                pkgMesh.mTangentData.push_back(transformedBitangents.z);
            }

            // mesh AABB
            pkgMesh.mAABB.mMinBounds = MinVal(pkgMesh.mAABB.mMinBounds, transformedVertices);
            pkgMesh.mAABB.mMaxBounds = MaxVal(pkgMesh.mAABB.mMaxBounds, transformedVertices);
        }

        pkgMesh.mIndiceData.reserve(mesh->mNumFaces * numFloatsPerTriangle);

        // index data
        for (uint32_t j = 0; j < mesh->mNumFaces; ++j)
        {
            // only dem triangles
            assert(mesh->mFaces[j].mNumIndices == numFloatsPerTriangle);
            for (uint32_t index = 0; index < numFloatsPerTriangle; index++)
            {
                assert(mesh->mFaces[j].mIndices[index] <= UINT16_MAX);
                pkgMesh.mIndiceData.push_back(mesh->mFaces[j].mIndices[index]);
            }
        }

        if (materialMap.find(mesh->mMaterialIndex) == materialMap.end())
        {
            Log("ERROR: Unable to find mesh material in materialMap");
            return false;
        }

        pkgMesh.mMaterialIndex = materialMap.at(mesh->mMaterialIndex);
        pkgMesh.mHasMaterial = true;

        // node AABB
        nodeMinBounds = MinVal(nodeMinBounds, pkgMesh.mAABB.mMinBounds);
        nodeMaxBounds = MaxVal(nodeMaxBounds, pkgMesh.mAABB.mMaxBounds);

        return true;
    }

    bool Assimp::ProcessAssimpAnimations(PackageModel& model, const aiScene* scene)
    {
        if (!scene->HasAnimations())
            return true;

        for (uint32_t animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex)
        {
            aiAnimation* animation = *(scene->mAnimations + animationIndex);

            // what is this anyway...
            // shouldn't be present
            assert(animation->mNumMeshChannels == 0);

            const uint32_t durationMillisec = static_cast<uint32_t>((animation->mDuration / animation->mTicksPerSecond) * 1000);
            model.mAnimations.emplace_back(animation->mName.C_Str(), durationMillisec);
            PackageAnimation& pkgAnimation = model.mAnimations.back();

            aiNode* rootNode = scene->mRootNode;
            const uint32_t noAnimationKeysNum = 1;
            for (uint32_t nodeKey = 0; nodeKey < animation->mNumChannels; ++nodeKey)
            {
                aiNodeAnim* nodeAnimation = *(animation->mChannels + nodeKey);

                // cant do scaling animations
                assert(nodeAnimation->mNumScalingKeys == noAnimationKeysNum);

                // if no rotation/translation, continue
                if (nodeAnimation->mNumPositionKeys == nodeAnimation->mNumRotationKeys == noAnimationKeysNum)
                    continue;

                const uint32_t nodeIndex = GetNodeIndex(model, nodeAnimation->mNodeName.C_Str());
                // index outside range means we didnt find node --> crash
                assert(nodeIndex < model.mNodes.size());

                pkgAnimation.mAnimatedNodes.emplace_back(nodeIndex);
                PackageAnimatedNode& pkgAnimatedNode = pkgAnimation.mAnimatedNodes.back();

                const uint32_t numPosKeys = nodeAnimation->mNumPositionKeys;
                const uint32_t numRotkeys = nodeAnimation->mNumRotationKeys;
                const uint32_t maxNumKeys = glm::max(nodeAnimation->mNumPositionKeys, nodeAnimation->mNumRotationKeys);
                for (uint32_t key = 0; key < maxNumKeys; ++key)
                {
                    // same pos/rot might be used for several pos/rot transforms

                    // position
                    const uint32_t posKey = key < numPosKeys ? key : numPosKeys - 1;
                    aiVectorKey* aiPos = nodeAnimation->mPositionKeys + posKey;
                    const Vec3 posVec = aiVec3ToJonsVec3(aiPos->mValue);
                    Mat4 transform = glm::translate(posVec);

                    // rotation
                    const uint32_t rotKey = key < numRotkeys ? key : numRotkeys - 1;
                    aiQuatKey* aiRot = nodeAnimation->mRotationKeys + rotKey;
                    const Quaternion rotQuat = aiQuatToJonsQuat(aiRot->mValue);
                    transform *= glm::toMat4(rotQuat);

                    const double maxKeyTimeSeconds = glm::max(aiPos->mTime, aiRot->mTime) / animation->mTicksPerSecond;
                    const uint32_t timestampMillisec = static_cast<uint32_t>(maxKeyTimeSeconds * 1000);

                    pkgAnimatedNode.mAnimationTransforms.emplace_back(timestampMillisec, transform);
                }
            }
        }

        return true;
    }

    void AddStaticAABB(PackageModel& model)
    {
        // two cases: animated vs static model
        const PackageAABB& rootNodeAABB = model.mNodes.front().mAABB;

        // static model: use root node AABB as its overall AABB
        if (model.mAnimations.empty())
        {
            model.mStaticAABB = rootNodeAABB;
            return;
        }

        // animated model: needs to transform all nodes using all the animations to find the maximum extents
        // results in a loose, static aabb
        Vec3 minExtent(rootNodeAABB.mMinBounds), maxExtent(rootNodeAABB.mMaxBounds);
        for (const PackageAnimation& animation : model.mAnimations)
        {
            for (const PackageAnimatedNode& animNode : animation.mAnimatedNodes)
            {
                const PackageNode& node = model.mNodes.at(animNode.mNodeIndex);
                for (const PackageAnimatedNodeTransform& transform : animNode.mAnimationTransforms)
                {
                    AABB aabb(node.mAABB.mMinBounds, node.mAABB.mMaxBounds);
                    aabb = aabb * transform.mTransform;

                    const Vec3 tempMin = aabb.Min(), tempMax = aabb.Max();
                    if (tempMin.x < minExtent.x) minExtent.x = tempMin.x;
                    if (tempMin.y < minExtent.y) minExtent.y = tempMin.y;
                    if (tempMin.z < minExtent.z) minExtent.z = tempMin.z;

                    if (tempMax.x > maxExtent.x) maxExtent.x = tempMax.x;
                    if (tempMax.y > maxExtent.y) maxExtent.y = tempMax.y;
                    if (tempMax.z > maxExtent.z) maxExtent.z = tempMax.z;
                }
            }
        }
        model.mStaticAABB.mMinBounds = minExtent;
        model.mStaticAABB.mMaxBounds = maxExtent;
    }


    void CheckForInvalidAABB(PackageAABB& aabb)
    {
        // resets AABB to zero length
        if (aabb.mMinBounds == Vec3(std::numeric_limits<float>::max()) && aabb.mMaxBounds == Vec3(std::numeric_limits<float>::lowest()))
        {
            aabb.mMinBounds = Vec3(0.0f);
            aabb.mMaxBounds = Vec3(0.0f);
        }
    }

    uint32_t GetNodeIndex(const PackageModel& model, const std::string& nodeName)
    {
        uint32_t ret = model.mNodes.size();
        for (const PackageNode& node : model.mNodes)
        {
            if (node.mName == nodeName)
            {
                ret = node.mNodeIndex;
                break;
            }
        }

        assert(ret != model.mNodes.size());

        return ret;
    }


    Mat4 aiMat4ToJonsMat4(const aiMatrix4x4& aiMat)
    {
        Mat4 jMat;

        jMat[0][0] = aiMat.a1; jMat[0][1] = aiMat.b1; jMat[0][2] = aiMat.c1; jMat[0][3] = aiMat.d1;
        jMat[1][0] = aiMat.a2; jMat[1][1] = aiMat.b2; jMat[1][2] = aiMat.c2; jMat[1][3] = aiMat.d2;
        jMat[2][0] = aiMat.a3; jMat[2][1] = aiMat.b3; jMat[2][2] = aiMat.c3; jMat[2][3] = aiMat.d3;
        jMat[3][0] = aiMat.a4; jMat[3][1] = aiMat.b4; jMat[3][2] = aiMat.c4; jMat[3][3] = aiMat.d4;

        return jMat;
    }

    Quaternion aiQuatToJonsQuat(const aiQuaternion& aiQuat)
    {
        return Quaternion(aiQuat.x, aiQuat.y, aiQuat.z, aiQuat.w);
    }

    Vec3 aiColor3DToJonsVec3(const aiColor3D& color)
    {
        return Vec3(color.r, color.g, color.b);
    }

    Vec3 aiVec3ToJonsVec3(const aiVector3D& vec)
    {
        return Vec3(vec.x, vec.y, vec.z);
    }
}
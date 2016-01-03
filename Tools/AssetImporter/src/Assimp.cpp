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

    bool OnlyOneNodePerMesh(const aiScene* scene);
    Mat4 GetMeshNodeTransform(const aiScene* scene, const aiNode* node, const uint32_t meshIndex, const Mat4& parentTransform);
    uint32_t CountMeshOccurances(const aiNode* node, const uint32_t aiMeshIndex, const bool recursive);
    uint32_t GetNodeIndex(const PackageModel& model, const std::string& nodeName);
    uint32_t CountChildren(const aiNode* node);

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
        if (!ProcessAssimpModel(scene, modelName, materialMap, pkg))
            return false;

        PackageModel& model = pkg->mModels.back();
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

    bool Assimp::ProcessAssimpModel(const aiScene* scene, const std::string& modelName, const MaterialMap& materialMap, JonsPackagePtr pkg)
    {
        pkg->mModels.emplace_back(modelName);
        PackageModel& model = pkg->mModels.back();

        // reserve storage
        const uint32_t numNodes = CountChildren(scene->mRootNode) + 1;
        model.mNodes.reserve(numNodes);
        model.mMeshes.reserve(scene->mNumMeshes);
        model.mAnimations.reserve(scene->mNumAnimations);

        // we assume only one node per mesh - means we can pre-transform vertices for bind pose, makes rendering static actors faster
        if (!OnlyOneNodePerMesh(scene))
        {
            Log("ERROR: Assimp parser assumes only one node per mesh");
            return false;
        }

        if (!ProcessAssimpMeshes(model.mMeshes, model.mBones, scene, materialMap))
            return false;

        // recursively go through assimp node tree
        const auto rootParentIndex = PackageNode::INVALID_NODE_INDEX;
        if (!ProcessAssimpNode(model.mNodes, model.mMeshes, scene, scene->mRootNode, rootParentIndex))
            return false;

        return true;
    }

    bool Assimp::ProcessAssimpNode(std::vector<JonsEngine::PackageNode>& nodeContainer, const std::vector<JonsEngine::PackageMesh>& meshContainer, const aiScene* scene, const aiNode* assimpNode, const JonsEngine::PackageNode::NodeIndex parentNodeIndex)
    {
        const uint32_t nodeIndex = nodeContainer.size();
        nodeContainer.emplace_back(assimpNode->mName.C_Str(), nodeIndex, parentNodeIndex);
        PackageNode& jonsNode = nodeContainer.back();

        // jonsPkg uses same mesh indices as aiScene
        for (uint32_t meshIndex = 0; meshIndex < assimpNode->mNumMeshes; ++meshIndex)
        {
            jonsNode.mMeshes.emplace_back(assimpNode->mMeshes[meshIndex]);

            const PackageMesh& mesh = meshContainer.at(meshIndex);
            jonsNode.mAABB.mMinBounds = MinVal(jonsNode.mAABB.mMinBounds, mesh.mAABB.mMinBounds);
            jonsNode.mAABB.mMaxBounds = MaxVal(jonsNode.mAABB.mMaxBounds, mesh.mAABB.mMaxBounds);
        }

        const uint32_t firstChildIndex = nodeContainer.size();
        for (uint32_t childIndex = 0; childIndex < assimpNode->mNumChildren; ++childIndex)
        {
            const aiNode* child = assimpNode->mChildren[childIndex];

            if (!ProcessAssimpNode(nodeContainer, meshContainer, scene, child, nodeIndex))
                return false;
        }
        const uint32_t lastChildIndex = nodeContainer.size() - 1;

        // update node AABB
        for (uint32_t childIndex = firstChildIndex; childIndex <= lastChildIndex; ++childIndex)
        {
            const PackageNode& child = nodeContainer.at(childIndex);
            jonsNode.mAABB.mMinBounds = MinVal(jonsNode.mAABB.mMinBounds, child.mAABB.mMinBounds);
            jonsNode.mAABB.mMaxBounds = MaxVal(jonsNode.mAABB.mMaxBounds, child.mAABB.mMaxBounds);
        }

        // if node has no legit AABB from either its own mesh or a childrens mesh, zero length it
        // is done after updating parent aabb bounds since the result might otherwise invalidate the parents aabb when comparing it against its other childrens
        CheckForInvalidAABB(jonsNode.mAABB);

        return true;
    }

    bool Assimp::ProcessAssimpMeshes(std::vector<JonsEngine::PackageMesh>& meshContainer, std::vector<JonsEngine::PackageBone>& boneContainer, const aiScene* scene, const MaterialMap& materialMap)
    {
        const uint32_t numFloatsPerTriangle = 3;
        const uint32_t numFloatsPerTexcoord = 2;

        for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
        {
            aiMesh* assimpMesh = scene->mMeshes[meshIndex];
            meshContainer.emplace_back(assimpMesh->mName.C_Str());
            PackageMesh& jonsMesh = meshContainer.back();

            if (!ProcessAssimpBones(boneContainer, assimpMesh))
                return false;

            // reserve storage
            jonsMesh.mVertexData.reserve(assimpMesh->mNumVertices * numFloatsPerTriangle);
            jonsMesh.mNormalData.reserve(assimpMesh->mNumVertices * numFloatsPerTriangle);
            jonsMesh.mTexCoordsData.reserve(assimpMesh->mNumVertices * numFloatsPerTexcoord);
            // store both tangents and bitangents in same buffer
            jonsMesh.mTangentData.reserve(assimpMesh->mNumVertices * numFloatsPerTriangle * 2);
            jonsMesh.mIndiceData.reserve(assimpMesh->mNumFaces * numFloatsPerTriangle);

            // get the transform of the node associated with this mesh to pre-multiply all the vertices
            const Mat4 nodeTransform = GetMeshNodeTransform(scene, scene->mRootNode, meshIndex, gIdentityMatrix);

            // vertice, normal, texcoord, tangents and bitangents data
            for (unsigned int j = 0; j < assimpMesh->mNumVertices; ++j)
            {
                const Vec3 transformedVertices = Vec3(nodeTransform * Vec4(aiVec3ToJonsVec3(assimpMesh->mVertices[j]), 1.0f));
                jonsMesh.mVertexData.push_back(transformedVertices.x);
                jonsMesh.mVertexData.push_back(transformedVertices.y);
                jonsMesh.mVertexData.push_back(transformedVertices.z);

                if (assimpMesh->HasNormals())
                {
                    const Vec3 transformedNormals = Vec3(nodeTransform * Vec4(aiVec3ToJonsVec3(assimpMesh->mNormals[j]), 0.0f));
                    jonsMesh.mNormalData.push_back(transformedNormals.x);
                    jonsMesh.mNormalData.push_back(transformedNormals.y);
                    jonsMesh.mNormalData.push_back(transformedNormals.z);
                }

                if (assimpMesh->HasTextureCoords(0))
                {
                    jonsMesh.mTexCoordsData.push_back(assimpMesh->mTextureCoords[0][j].x);
                    jonsMesh.mTexCoordsData.push_back(assimpMesh->mTextureCoords[0][j].y);
                }

                if (assimpMesh->HasTangentsAndBitangents())
                {
                    const Vec3 transformedTangents = Vec3(nodeTransform * Vec4(aiVec3ToJonsVec3(assimpMesh->mTangents[j]), 0.0f));
                    jonsMesh.mTangentData.push_back(transformedTangents.x);
                    jonsMesh.mTangentData.push_back(transformedTangents.y);
                    jonsMesh.mTangentData.push_back(transformedTangents.z);

                    const Vec3 transformedBitangents = Vec3(nodeTransform * Vec4(aiVec3ToJonsVec3(assimpMesh->mBitangents[j]), 0.0f));
                    jonsMesh.mTangentData.push_back(transformedBitangents.x);
                    jonsMesh.mTangentData.push_back(transformedBitangents.y);
                    jonsMesh.mTangentData.push_back(transformedBitangents.z);
                }

                // mesh AABB
                jonsMesh.mAABB.mMinBounds = MinVal(jonsMesh.mAABB.mMinBounds, transformedVertices);
                jonsMesh.mAABB.mMaxBounds = MaxVal(jonsMesh.mAABB.mMaxBounds, transformedVertices);
            }

            // index data
            for (uint32_t j = 0; j < assimpMesh->mNumFaces; ++j)
            {
                // only dem triangles
                assert(assimpMesh->mFaces[j].mNumIndices == numFloatsPerTriangle);
                for (uint32_t index = 0; index < numFloatsPerTriangle; index++)
                {
                    assert(assimpMesh->mFaces[j].mIndices[index] <= UINT16_MAX);
                    jonsMesh.mIndiceData.push_back(assimpMesh->mFaces[j].mIndices[index]);
                }
            }

            if (materialMap.find(assimpMesh->mMaterialIndex) == materialMap.end())
            {
                Log("ERROR: Unable to find mesh material in materialMap");
                return false;
            }

            jonsMesh.mMaterialIndex = materialMap.at(assimpMesh->mMaterialIndex);
            jonsMesh.mHasMaterial = true;
        }

        return true;
    }

    bool Assimp::ProcessAssimpBones(std::vector<JonsEngine::PackageBone>& boneContainer, const aiMesh* assimpMesh)
    {
        const auto numBones = assimpMesh->mNumBones;
        for (uint32_t index = 0; index < numBones; ++index)
        {
            const auto bone = assimpMesh->mBones[index];

            auto asd = bone->mWeights[index];
            if (asd.mWeight < 1.0f)
                auto aw = 1.0f;
        }

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

    bool OnlyOneNodePerMesh(const aiScene* scene)
    {
        for (uint32_t meshNum = 0; meshNum < scene->mNumMeshes; ++meshNum)
        {
            const bool recursive = true;
            const uint32_t occurances = CountMeshOccurances(scene->mRootNode, meshNum, recursive);
            if (occurances > 1)
                return false;
        }

        return true;
    }

    // assumes aiMeshIndex actually exists in node hierarchy
    Mat4 GetMeshNodeTransform(const aiScene* scene, const aiNode* node, const uint32_t aiMeshIndex, const Mat4& parentTransform)
    {
        const bool recursive = false;
        const uint32_t occurances = CountMeshOccurances(node, aiMeshIndex, recursive);
        const Mat4 nodeTransform = parentTransform * aiMat4ToJonsMat4(node->mTransformation);
        if (occurances != 1)
        {
            for (uint32_t childNum = 0; childNum < node->mNumChildren; ++childNum)
            {
                const aiNode* child = node->mChildren[childNum];
                if (CountMeshOccurances(child, aiMeshIndex, recursive) == 1)
                    return GetMeshNodeTransform(scene, child, aiMeshIndex, nodeTransform);
            }
        }

        return nodeTransform;
    }

    uint32_t CountMeshOccurances(const aiNode* node, const uint32_t aiMeshIndex, const bool recursive)
    {
        uint32_t occurances = 0;
        for (uint32_t meshNum = 0; meshNum < node->mNumMeshes; ++meshNum)
        {
            if (*(node->mMeshes + meshNum) == aiMeshIndex)
                ++occurances;
        }

        if (recursive)
        {
            for (uint32_t childNum = 0; childNum < node->mNumChildren; ++childNum)
            {
                const aiNode* child = node->mChildren[childNum];
                occurances += CountMeshOccurances(child, aiMeshIndex, recursive);
            }
        }

        return occurances;
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

    uint32_t CountChildren(const aiNode* node)
    {
        uint32_t ret = 0;

        for (uint32_t index = 0; index < node->mNumChildren; ++index)
        {
            ++ret;

            ret += CountChildren(node->mChildren[index]);
        }

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
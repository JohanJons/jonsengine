#include "include/Assimp.h"

#include "include/Core/Math/Math.h"
#include "include/Core/Math/AABB.h"
#include "include/Resources/Animation.h"
#include "include/FreeImage.h"

#include <limits>
#include <set>
#include <algorithm>

using namespace JonsEngine;

namespace JonsAssetImporter
{
	typedef std::set<std::string> BoneNameSet;
	typedef std::set<const aiBone*> AssimpBoneSet;

    void AddStaticAABB(PackageModel& model);

    bool OnlyOneNodePerMesh(const aiScene* scene);
    Mat4 GetMeshNodeTransform(const aiScene* scene, const aiNode* node, const uint32_t meshIndex, const Mat4& parentTransform);
    uint32_t CountMeshOccurances(const aiNode* node, const uint32_t aiMeshIndex, const bool recursive);
    BoneIndex GetBoneIndex(const PackageModel& model, const std::string& boneName);
    uint32_t CountChildren(const aiNode* node);
    bool UsedLessThanMaxNumBones(const BoneWeight& pkgBoneWeight);
    const aiNode* FindMeshNode(const aiMesh* mesh, const aiScene* scene, const aiNode* node);
    Vec3 GetVertices(const bool isStatic, const Mat4& nodeTransform, const aiVector3D& assimpVertices);
	PackageAnimation& AddPkgAnimation(PackageModel& model, const aiAnimation* animation);
	PackageAnimation::KeyFrameContainer& GetBoneKeyframeContainer(PackageAnimation& pkgAnimation, const std::vector<PackageBone>& bones, const aiNodeAnim* nodeAnimation);
	void BuildSkeleton(BoneParentMap& parentMap, std::vector<PackageBone>& skeleton, const BoneNameSet& boneNames, const AssimpBoneSet& aiBones, const aiNode* node, const Mat4& parentTransform, const BoneIndex parentBone);

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
        ProcessMaterials(scene, modelPath, materialMap, freeimageImporter, pkg);

        // process model hierarchy
        if (!ParseModel(scene, modelName, materialMap, pkg))
            return false;

        PackageModel& model = pkg->mModels.back();
        if (!ProcessAnimations(model, scene))
            return false;

        //AddStaticAABB(model);

        return true;
    }

    void Assimp::ProcessMaterials(const aiScene* scene, const boost::filesystem::path& modelPath, MaterialMap& materialMap, FreeImage& freeimageImporter, JonsPackagePtr pkg)
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

    bool Assimp::ParseModel(const aiScene* scene, const std::string& modelName, const MaterialMap& materialMap, JonsPackagePtr pkg)
    {
        pkg->mModels.emplace_back(modelName);
        PackageModel& model = pkg->mModels.back();

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

        if (!ProcessMeshes(model.mMeshes, scene, materialMap, model.mStaticAABB.mMinBounds, model.mStaticAABB.mMaxBounds))
            return false;

        // recursively go through assimp node tree
        const auto rootParentIndex = PackageNode::INVALID_NODE_INDEX;
        if (!ParseNodeHeirarchy(model.mNodes, model.mMeshes, scene, scene->mRootNode, rootParentIndex, gIdentityMatrix))
            return false;

        if (!ProcessBones(model.mBoneParentMap, model.mSkeleton, scene))
            return false;

        return true;
    }

    bool Assimp::ParseNodeHeirarchy(std::vector<PackageNode>& nodeContainer, const std::vector<PackageMesh>& meshContainer, const aiScene* scene, const aiNode* assimpNode, const PackageNode::NodeIndex parentNodeIndex, const Mat4& parentTransform)
    {
        const uint32_t nodeIndex = nodeContainer.size();
		const Mat4 nodeTransform = parentTransform * aiMat4ToJonsMat4(assimpNode->mTransformation);
        nodeContainer.emplace_back(assimpNode->mName.C_Str(), nodeTransform, nodeIndex, parentNodeIndex);
        PackageNode& jonsNode = nodeContainer.back();

        // jonsPkg uses same mesh indices as aiScene
        for (uint32_t meshIndex = 0; meshIndex < assimpNode->mNumMeshes; ++meshIndex)
            jonsNode.mMeshes.emplace_back(assimpNode->mMeshes[meshIndex]);

        for (uint32_t childIndex = 0; childIndex < assimpNode->mNumChildren; ++childIndex)
        {
            const aiNode* child = assimpNode->mChildren[childIndex];

            if (!ParseNodeHeirarchy(nodeContainer, meshContainer, scene, child, nodeIndex, nodeTransform))
                return false;
        }

        return true;
    }

    bool Assimp::ProcessMeshes(std::vector<PackageMesh>& meshContainer, const aiScene* scene, const MaterialMap& materialMap, Vec3& modelMinBounds, Vec3& modelMaxBounds)
    {
        for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
        {
            aiMesh* assimpMesh = scene->mMeshes[meshIndex];
            meshContainer.emplace_back(assimpMesh->mName.C_Str());
            PackageMesh& jonsMesh = meshContainer.back();
            
            if (!AddMeshGeometricData(jonsMesh, assimpMesh, scene, meshIndex, modelMinBounds, modelMaxBounds))
                return false;

            // bone weights
            if (!ProcessVertexBoneWeights(jonsMesh.mBoneWeights, assimpMesh))
                return false;

            if (materialMap.find(assimpMesh->mMaterialIndex) == materialMap.end())
            {
                Log("ERROR: Unable to find mesh material in materialMap");
                return false;
            }

            jonsMesh.mMaterialIndex = materialMap.at(assimpMesh->mMaterialIndex);
        }

        return true;
    }

    // the bones of jonsMesh must've been parsed already
    bool Assimp::AddMeshGeometricData(PackageMesh& jonsMesh, const aiMesh* assimpMesh, const aiScene* scene, const uint32_t meshIndex, Vec3& modelMinBounds, Vec3& modelMaxBounds)
    {
        const uint32_t numFloatsPerTriangle = 3;
        const uint32_t numFloatsPerTexcoord = 2;

        // reserve storage
        jonsMesh.mVertexData.reserve(assimpMesh->mNumVertices * numFloatsPerTriangle);
        jonsMesh.mNormalData.reserve(assimpMesh->mNumVertices * numFloatsPerTriangle);
        jonsMesh.mTexCoordsData.reserve(assimpMesh->mNumVertices * numFloatsPerTexcoord);
        // store both tangents and bitangents in same buffer
        jonsMesh.mTangentData.reserve(assimpMesh->mNumVertices * numFloatsPerTriangle * 2);
        jonsMesh.mIndiceData.reserve(assimpMesh->mNumFaces * numFloatsPerTriangle);

        // NOTE: commented pre-transform code out, might need some additional thinking to get it right (inverse transforms for animated meshes?)
        // if the mesh is static, pre-multiply all the vertices with the transformation hierarchy
        //const bool isStatic = !jonsMesh.IsAnimated();
        //const Mat4 nodeTransform = isStatic ? GetMeshNodeTransform(scene, scene->mRootNode, meshIndex, gIdentityMatrix) : gIdentityMatrix;
		const Mat4 nodeTransform = GetMeshNodeTransform(scene, scene->mRootNode, meshIndex, gIdentityMatrix);

        // vertice, normal, texcoord, tangents and bitangents data
        for (uint32_t j = 0; j < assimpMesh->mNumVertices; ++j)
        {
            //const Vec3 vertices = GetVertices(isStatic, nodeTransform, assimpMesh->mVertices[j]);
            const Vec3 vertices = aiVec3ToJonsVec3(assimpMesh->mVertices[j]);
            jonsMesh.mVertexData.push_back(vertices.x);
            jonsMesh.mVertexData.push_back(vertices.y);
            jonsMesh.mVertexData.push_back(vertices.z);

            if (assimpMesh->HasNormals())
            {
                //const Vec3 normals = GetVertices(isStatic, nodeTransform, assimpMesh->mNormals[j]);
                const Vec3 normals = aiVec3ToJonsVec3(assimpMesh->mNormals[j]);
                jonsMesh.mNormalData.push_back(normals.x);
                jonsMesh.mNormalData.push_back(normals.y);
                jonsMesh.mNormalData.push_back(normals.z);
            }

            // multiple texture coordinates only used in special scenarios so only use first row by default
            if (assimpMesh->HasTextureCoords(0))
            {
                jonsMesh.mTexCoordsData.push_back(assimpMesh->mTextureCoords[0][j].x);
                jonsMesh.mTexCoordsData.push_back(assimpMesh->mTextureCoords[0][j].y);
            }

            if (assimpMesh->HasTangentsAndBitangents())
            {
                //const Vec3 tangents = GetVertices(isStatic, nodeTransform, assimpMesh->mTangents[j]);
                const Vec3 tangents = aiVec3ToJonsVec3(assimpMesh->mTangents[j]);
                jonsMesh.mTangentData.push_back(tangents.x);
                jonsMesh.mTangentData.push_back(tangents.y);
                jonsMesh.mTangentData.push_back(tangents.z);

                //const Vec3 bitangents = GetVertices(isStatic, nodeTransform, assimpMesh->mBitangents[j]);
                const Vec3 bitangents = aiVec3ToJonsVec3(assimpMesh->mBitangents[j]);
                jonsMesh.mTangentData.push_back(bitangents.x);
                jonsMesh.mTangentData.push_back(bitangents.y);
                jonsMesh.mTangentData.push_back(bitangents.z);
            }

            // mesh AABB
			// pre-multiply transforms for static aabb
			const Vec3 transformedVertices = Vec3(nodeTransform * Vec4(vertices, 1.0f));
			modelMinBounds = MinVal(modelMinBounds, transformedVertices);
			modelMaxBounds = MaxVal(modelMaxBounds, transformedVertices);
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

        return true;
    }

    const aiBone* FindAiBoneByName(const std::set<const aiBone*> aiBones, const std::string& string)
    {
        for (const aiBone* bone : aiBones)
        {
            if (bone->mName.C_Str() == string)
                return bone;
        }
    
        return nullptr;
    }

    bool Assimp::ProcessBones(JonsEngine::BoneParentMap& parentMap, std::vector<PackageBone>& bones, const aiScene* scene)
    {
        BoneNameSet boneNames;
        AssimpBoneSet aiBones;

        for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
        {
            const aiMesh* mesh = scene->mMeshes[meshIndex];
            assert(mesh);
            const aiNode* meshNode = FindMeshNode(mesh, scene, scene->mRootNode);
            assert(meshNode);
            const aiNode* parentMeshNode = meshNode->mParent;
			assert(parentMeshNode);
            
            for (uint32_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
            {
                const aiBone* bone = mesh->mBones[boneIndex];
                assert(bone);
                const aiNode* node = scene->mRootNode->FindNode(bone->mName.C_Str());
                assert(node);
                
                aiBones.insert(bone);
                do
                {
                    boneNames.insert(node->mName.C_Str());
                    node = node->mParent;
                }
                while (node && node != meshNode && node != parentMeshNode);
            }
        }

        const std::size_t numBones = boneNames.size();
        parentMap.resize(numBones, INVALID_BONE_INDEX);
        bones.reserve(numBones);

        BuildSkeleton(parentMap, bones, boneNames, aiBones, scene->mRootNode, gIdentityMatrix, INVALID_BONE_INDEX);
        assert(bones.size() == numBones);

        return true;
    }

    bool Assimp::ProcessVertexBoneWeights(std::vector<BoneWeight>& boneWeights, const aiMesh* assimpMesh)
    {
        // make sure containers are large enough as we will access indices directly when iterating the bones
        const uint32_t numVertices = assimpMesh->mNumVertices;
		boneWeights.resize(numVertices);

		const uint32_t numBones = assimpMesh->mNumBones;
		assert(numBones <= MAX_NUM_BONES);
		for (BoneIndex bone = 0; bone < numBones; ++bone)
		{
			const auto assimpBone = assimpMesh->mBones[bone];
            const uint32_t numWeights = assimpBone->mNumWeights;

            for (uint32_t weightIndex = 0; weightIndex < numWeights; ++weightIndex)
            {
                const auto assimpWeight = assimpBone->mWeights[weightIndex];
                const uint32_t vertexID = assimpWeight.mVertexId;
				const float weightFactor = assimpWeight.mWeight;
                
                auto& pkgWeight = boneWeights.at(vertexID);
                const bool notExceededNumBones = UsedLessThanMaxNumBones(pkgWeight);
				assert(notExceededNumBones);
                
				uint32_t firstFreeIndex = 0;
				while (pkgWeight.mBoneIndices.at(firstFreeIndex) != INVALID_BONE_INDEX)
					++firstFreeIndex;

				pkgWeight.mBoneIndices.at(firstFreeIndex) = bone;
				pkgWeight.mBoneWeights.at(firstFreeIndex) = weightFactor;
            }
		}
        
        return true;
    }

    bool Assimp::ProcessAnimations(PackageModel& model, const aiScene* scene)
    {
        if (!scene->HasAnimations())
            return true;

        for (uint32_t animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex)
        {
            aiAnimation* animation = *(scene->mAnimations + animationIndex);
			assert(animation);

            // what is this anyway... shouldn't be present
            assert(animation->mNumMeshChannels == 0);

			PackageAnimation& pkgAnimation = AddPkgAnimation(model, animation);

            const uint32_t noAnimationKeysNum = 1;
            for (uint32_t nodeAnimIndex = 0; nodeAnimIndex < animation->mNumChannels; ++nodeAnimIndex)
            {
                aiNodeAnim* nodeAnimation = *(animation->mChannels + nodeAnimIndex);
				assert(nodeAnimation);

                // cant do scaling animations
                assert(nodeAnimation->mNumScalingKeys == noAnimationKeysNum);

                // if no rotation and translation, continue
                if (nodeAnimation->mNumPositionKeys == nodeAnimation->mNumRotationKeys == noAnimationKeysNum)
                    continue;
				
				PackageAnimation::KeyFrameContainer& keyFrames = GetBoneKeyframeContainer(pkgAnimation, model.mSkeleton, nodeAnimation);

                const uint32_t numPosKeys = nodeAnimation->mNumPositionKeys;
                const uint32_t numRotkeys = nodeAnimation->mNumRotationKeys;
                const uint32_t maxNumKeys = glm::max(nodeAnimation->mNumPositionKeys, nodeAnimation->mNumRotationKeys);
                for (uint32_t key = 0; key < maxNumKeys; ++key)
                {
                    // same pos/rot might be used for several pos/rot transforms
                    // NOTE: use mTransformation after last key encountered????

                    // position
                    const uint32_t posKey = key < numPosKeys ? key : numPosKeys - 1;
                    aiVectorKey* aiPos = nodeAnimation->mPositionKeys + posKey;
					assert(aiPos);
                    const Vec3 translateVector = aiVec3ToJonsVec3(aiPos->mValue);

                    // rotation
                    const uint32_t rotKey = key < numRotkeys ? key : numRotkeys - 1;
                    aiQuatKey* aiRot = nodeAnimation->mRotationKeys + rotKey;
					assert(aiRot);
                    const Quaternion rotationQuat = aiQuatToJonsQuat(aiRot->mValue);

                    const double maxKeyTimeSeconds = glm::max(aiPos->mTime, aiRot->mTime) / animation->mTicksPerSecond;
                    const uint32_t timestampMillisec = static_cast<uint32_t>(maxKeyTimeSeconds * 1000);

					keyFrames.emplace_back(timestampMillisec, translateVector, rotationQuat);
                }
            }
        }

        return true;
    }

    void AddStaticAABB(PackageModel& model)
    {
        // two cases: animated vs static model
        //const PackageAABB& rootNodeAABB = model.mNodes.front().mAABB;
        //model.mStaticAABB = rootNodeAABB;

        // TODO
        // static model: use root node AABB as its overall AABB
       /* if (model.mAnimations.empty())
        {
            model.mStaticAABB = rootNodeAABB;
            return;
        }

        // animated model: needs to transform all nodes using all the animations to find the maximum extents
        // results in a loose, static aabb
        Vec3 minExtent(rootNodeAABB.mMinBounds), maxExtent(rootNodeAABB.mMaxBounds);
        for (const PackageAnimation& animation : model.mAnimations)
        {
            for (const PackageBoneAnimation& animNode : animation.mBoneAnimations)
            {
                for (const PackageBoneKeyframe& keyframe : animNode.mKeyframes)
                {
                    AABB aabb(node.mAABB.mMinBounds, node.mAABB.mMaxBounds);
                    aabb = aabb * keyframe.mTransform;

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
        model.mStaticAABB.mMaxBounds = maxExtent;*/
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
    
    BoneIndex GetBoneIndex(const PackageModel& model, const std::string& boneName)
    {
        for (const PackageMesh& mesh : model.mMeshes)
        {
            const uint32_t numBones = model.mSkeleton.size();
            for (BoneIndex boneIndex = 0; boneIndex < numBones; ++boneIndex)
            {
                const PackageBone& bone = model.mSkeleton.at(boneIndex);
                if (bone.mName == boneName)
                    return boneIndex;
            }
        }
        
        return INVALID_BONE_INDEX;
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

    bool UsedLessThanMaxNumBones(const BoneWeight& pkgBoneWeight)
    {
        for (uint32_t bone = 0; bone < MAX_BONES_PER_VERTEX; ++bone)
        {
            // invalid index means unused
            if (pkgBoneWeight.mBoneIndices.at(bone) == INVALID_BONE_INDEX)
                return true;
        }
        
        return false;
    }

    const aiNode* FindMeshNode(const aiMesh* mesh, const aiScene* scene, const aiNode* node)
    {
        assert(mesh);
        assert(scene);
        assert(node);

        for (uint32_t meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex)
        {
            const aiMesh* nodeMesh = scene->mMeshes[meshIndex];
            if (nodeMesh == mesh)
                return node;
        }

        for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
        {
            const aiNode* childNode = node->mChildren[childIndex];
            assert(childNode);
            const aiNode* childRes = FindMeshNode(mesh, scene, childNode);
            if (childRes != nullptr)
                return childRes;
        }
        
        return nullptr;
    }
    
    Vec3 GetVertices(const bool isStaticMesh, const Mat4& nodeTransform, const aiVector3D& assimpVertices)
    {
        Vec3 ret = aiVec3ToJonsVec3(assimpVertices);
        if (isStaticMesh)
            ret = Vec3(nodeTransform * Vec4(ret, 1.0f));

        return ret;
    }

	PackageAnimation& AddPkgAnimation(PackageModel& model, const aiAnimation* animation)
	{
		const Mat4& rootNodeTransform = model.mNodes.front().mTransform;
		const Mat4 invRootNodeTransform = glm::inverse(rootNodeTransform);

		const uint32_t durationMillisec = static_cast<uint32_t>((animation->mDuration / animation->mTicksPerSecond) * 1000);
		model.mAnimations.emplace_back(animation->mName.C_Str(), durationMillisec, invRootNodeTransform);
		PackageAnimation& pkgAnimation = model.mAnimations.back();

		// number of nodeanim channels should be same as num bones
		// TODO: vector of vectors - rearrange boneanims for better cache utilization?
		const uint32_t numBones = model.mSkeleton.size();
		pkgAnimation.mBoneAnimations.resize(numBones);

		return pkgAnimation;
	}

	PackageAnimation::KeyFrameContainer& GetBoneKeyframeContainer(PackageAnimation& pkgAnimation, const std::vector<PackageBone>& bones, const aiNodeAnim* nodeAnimation)
	{
		const std::string nodeAnimName = nodeAnimation->mNodeName.C_Str();
		const auto boneEndIter = bones.end();
		const auto boneIter = std::find_if(bones.begin(), boneEndIter, [&nodeAnimName](const PackageBone& pkgBone) { return pkgBone.mName == nodeAnimName; });
		assert(boneIter != boneEndIter);
		const BoneIndex bone = boneIter - bones.begin();
		
		return pkgAnimation.mBoneAnimations.at(bone);
	}

	void BuildSkeleton(BoneParentMap& parentMap, std::vector<PackageBone>& skeleton, const BoneNameSet& boneNames, const AssimpBoneSet& aiBones, const aiNode* node, const Mat4& parentTransform, const BoneIndex parentBone)
	{
		BoneIndex thisBone = INVALID_BONE_INDEX;
		const Mat4 nodeTransform = parentTransform * aiMat4ToJonsMat4(node->mTransformation);
		const auto boneNameIter = boneNames.find(node->mName.C_Str());
		if (boneNameIter != boneNames.end())
		{
			const std::string& boneName = *boneNameIter;
			const aiBone* bone = FindAiBoneByName(aiBones, boneName);
			if (bone)
                skeleton.emplace_back(boneName, aiMat4ToJonsMat4(bone->mOffsetMatrix));
            else
                skeleton.emplace_back(boneName, gIdentityMatrix);
                //skeleton.emplace_back(boneName, nodeTransform);

			thisBone = static_cast<BoneIndex>(skeleton.size() - 1);
			parentMap.at(thisBone) = parentBone;

			// parents must always appear infront of children to speed up updating transforms
			assert(parentBone == INVALID_BONE_INDEX || parentBone < thisBone);
		}

		for (uint32_t childIndex = 0; childIndex < node->mNumChildren; childIndex++)
		{
			const aiNode* childNode = node->mChildren[childIndex];
			BuildSkeleton(parentMap, skeleton, boneNames, aiBones, childNode, nodeTransform, thisBone);
		}
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
        return Quaternion(aiQuat.w, aiQuat.x, aiQuat.y, aiQuat.z);
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
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

	bool ImportTexture(PackageTexture& texture, const boost::filesystem::path& modelPath, const std::string& texturePathStr, FreeImage& freeimageImporter);

    bool OnlyOneNodePerMesh(const aiScene* scene);
    Mat4 GetMeshNodeTransform(const aiScene* scene, const aiNode* node, const uint32_t meshIndex, const Mat4& parentTransform);
    uint32_t CountMeshOccurances(const aiNode* node, const uint32_t aiMeshIndex, const bool recursive);
    BoneIndex GetBoneIndex(const std::vector<PackageMesh>& meshes, const std::vector<PackageBone>& bones, const std::string& boneName);
    uint32_t CountChildren(const aiNode* node);
    bool UsedLessThanMaxNumBones(const BoneWeight& pkgBoneWeight);
	template <typename T>
	bool DoesPkgNameExist(const std::vector<T>& pkgContainer, const std::string& name);
    const aiNode* FindMeshNode(const aiMesh* mesh, const aiScene* scene, const aiNode* node);
	const aiMesh* FindMesh(const aiScene* scene, const MeshNameMap& meshNameMap, const std::string& pkgName);
	const aiBone* FindAiBoneByName(const std::set<const aiBone*> aiBones, const std::string& string);
    Vec3 GetVertices(const bool isStatic, const Mat4& nodeTransform, const aiVector3D& assimpVertices);
    
	PackageAnimation& AddPkgAnimation(PackageModel& model, const aiScene* scene, const aiAnimation* animation, uint32_t& unnamedAnimationCounter);
	BoneIndex GetBoneKeyframeContainer(PackageAnimation& pkgAnimation, const std::vector<PackageBone>& bones, const aiNodeAnim* nodeAnimation);
	void BuildSkeleton(BoneParentMap& parentMap, std::vector<PackageBone>& skeleton, const BoneNameSet& boneNames, const AssimpBoneSet& aiBones, const aiNode* node, const BoneIndex parentBone);
	std::set<std::string> GetListOfAnimatedNodes(const aiScene* scene);
    void ParseStaticAABBForAnimatedModel(PackageModel& model);
	void UpdateBoneTransforms(const Milliseconds currTimestamp, const Animation& jonsAnimation, BoneTransforms& boneTransforms, const std::size_t numBones);
	void TransformsVerticesAndCheckAABB(PackageModel& model, const BoneTransforms& boneTransforms, Vec3& minExtent, Vec3& maxExtent);

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
		if (!ProcessMaterials(pkg->mMaterials, scene, modelPath, materialMap, freeimageImporter))
			return false;

        // process model hierarchy
        if (!ParseModel(scene, modelName, materialMap, pkg))
            return false;

        PackageModel& model = pkg->mModels.back();
        if (!ProcessAnimations(model, scene))
            return false;

        ParseStaticAABBForAnimatedModel(model);

        return true;
    }

    bool Assimp::ProcessMaterials(std::vector<PackageMaterial>& materials, const aiScene* scene, const boost::filesystem::path& modelPath, MaterialMap& materialMap, FreeImage& freeimageImporter)
    {
        if (!scene->HasMaterials())
            return true;

		uint32_t unnamedMaterialCounter = 0;
        for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
        {
            const aiMaterial* material = scene->mMaterials[i];

			aiString diffuseTexturePath("");
			aiString normalTexturePath("");
			const bool hasDiffuseTexture = material->GetTextureCount(aiTextureType_DIFFUSE) > 0 && material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuseTexturePath) == aiReturn_SUCCESS;
			const bool hasNormalTexture = (material->GetTextureCount(aiTextureType_NORMALS) > 0 && material->GetTexture(aiTextureType_NORMALS, 0, &normalTexturePath) == aiReturn_SUCCESS) ||
										  (material->GetTextureCount(aiTextureType_HEIGHT) > 0 && material->GetTexture(aiTextureType_HEIGHT, 0, &normalTexturePath) == aiReturn_SUCCESS);

			aiString assimpMaterialName("");
			material->Get(AI_MATKEY_NAME, assimpMaterialName);

			std::string materialName(assimpMaterialName.C_Str());
			bool materialAlreadyImported = DoesPkgNameExist<PackageMaterial>(materials, materialName);
			if (materialAlreadyImported)
				Log("WARNING: Name collision for material " + materialName);

			// name collision - need to rename material
			while (materialName.empty() || materialAlreadyImported)
			{
				materialName = "Material_" + std::to_string(unnamedMaterialCounter++);
				materialAlreadyImported = DoesPkgNameExist<PackageMaterial>(materials, materialName);
			}

			materials.emplace_back(materialName, hasDiffuseTexture, hasNormalTexture);
			PackageMaterial& pkgMaterial = materials.back();

			if (hasDiffuseTexture)
			{
				if (!ImportTexture(pkgMaterial.mDiffuseTexture, modelPath, diffuseTexturePath.C_Str(), freeimageImporter))
				{
					Log("ERROR: Unable to import diffuse texture " + pkgMaterial.mName);
					return false;
				}
			}
			if (hasNormalTexture)
			{
				if (!ImportTexture(pkgMaterial.mNormalTexture, modelPath, normalTexturePath.C_Str(), freeimageImporter))
				{
					Log("ERROR: Unable to import normal texture " + pkgMaterial.mName);
					return false;
				}
			}

            aiColor3D diffuseColor;
            aiColor3D ambientColor(0.1f);
            aiColor3D specularColor;
            aiColor3D emissiveColor;
            material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
            //if (material->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor) == aiReturn_SUCCESS)     TODO
            //    ambientColor = aiColor3D(1.0f);
            material->Get(AI_MATKEY_COLOR_SPECULAR, specularColor);
            material->Get(AI_MATKEY_COLOR_EMISSIVE, emissiveColor);
            pkgMaterial.mDiffuseColor = aiColor3DToJonsVec3(diffuseColor);
            pkgMaterial.mAmbientColor = aiColor3DToJonsVec3(ambientColor);
            pkgMaterial.mSpecularColor = aiColor3DToJonsVec3(specularColor);
            pkgMaterial.mEmissiveColor = aiColor3DToJonsVec3(emissiveColor);

			const uint32_t pkgMaterialIndex = materials.size() - 1;
            materialMap.insert(MaterialPair(i, pkgMaterialIndex));
        }

		return true;
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

		// need to map assimp name to pkgName since we need to name them in case of name collisions
		MeshNameMap meshNameMap;
        if (!ProcessMeshes(model.mMeshes, scene, materialMap, meshNameMap, model.mStaticAABB.mMinBounds, model.mStaticAABB.mMaxBounds))
            return false;

        // recursively go through assimp node tree
        const auto rootParentIndex = PackageNode::INVALID_NODE_INDEX;
        if (!ParseNodeHeirarchy(model.mNodes, model.mMeshes, scene, scene->mRootNode, rootParentIndex, gIdentityMatrix))
            return false;

        if (!ProcessBones(model.mBoneParentMap, model.mSkeleton, model.mMeshes, meshNameMap, scene))
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

    bool Assimp::ProcessMeshes(std::vector<PackageMesh>& meshContainer, const aiScene* scene, const MaterialMap& materialMap, MeshNameMap& meshNameMap, Vec3& modelMinBounds, Vec3& modelMaxBounds)
    {
		uint32_t unnamedMeshCounter = 0;
        for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
        {
            aiMesh* assimpMesh = scene->mMeshes[meshIndex];
			std::string assimpName = assimpMesh->mName.C_Str();
			std::string meshName = assimpName;
			bool meshAlreadyImported = DoesPkgNameExist<PackageMesh>(meshContainer, meshName);
			
			// name collision - need to rename mesh
			while (meshName.empty() || meshAlreadyImported)
			{
				meshName = "Mesh_" + std::to_string(unnamedMeshCounter++);
				meshAlreadyImported = DoesPkgNameExist<PackageMesh>(meshContainer, meshName);
			}

			meshNameMap.emplace(meshName, assimpName);
            meshContainer.emplace_back(meshName);
            PackageMesh& jonsMesh = meshContainer.back();
            
            if (!AddMeshGeometricData(jonsMesh, assimpMesh, scene, meshIndex))
                return false;

			// update overall model AABB
			modelMinBounds = MinVal(modelMinBounds, jonsMesh.mAABB.mMinBounds);
			modelMaxBounds = MaxVal(modelMaxBounds, jonsMesh.mAABB.mMaxBounds);
           
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
    bool Assimp::AddMeshGeometricData(PackageMesh& jonsMesh, const aiMesh* assimpMesh, const aiScene* scene, const uint32_t meshIndex)
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
			// pre-multiply transforms for static aabb, since we just test against the overall models AABB for culling etc
			const Vec3 transformedVertices = Vec3(nodeTransform * Vec4(vertices, 1.0f));
			jonsMesh.mAABB.mMinBounds = MinVal(jonsMesh.mAABB.mMinBounds, transformedVertices);
			jonsMesh.mAABB.mMaxBounds = MaxVal(jonsMesh.mAABB.mMaxBounds, transformedVertices);
        }

        // index data
        for (uint32_t j = 0; j < assimpMesh->mNumFaces; ++j)
        {
			const aiFace face = assimpMesh->mFaces[j];

			// only dem triangles
			// should mNumIndices != 3, we can just ignore it, if we have provided #aiProcess_Triangulate and #aiProcess_SortByPType, according to assimp docs
			if (face.mNumIndices != numFloatsPerTriangle)
				continue;

            for (uint32_t index = 0; index < numFloatsPerTriangle; ++index)
            {
                assert(face.mIndices[index] <= UINT16_MAX);
                jonsMesh.mIndiceData.push_back(face.mIndices[index]);
            }
        }

        return true;
    }

    bool Assimp::ProcessBones(JonsEngine::BoneParentMap& parentMap, std::vector<PackageBone>& bones, std::vector<PackageMesh>& meshes, const MeshNameMap& meshNameMap, const aiScene* scene)
    {
        std::set<std::string> boneNames;
        std::set<const aiBone*> aiBones;

		auto animatedNodesSet = GetListOfAnimatedNodes(scene);
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
					// only nodes that are animated can be considered bones
					const std::string nodeName = node->mName.C_Str();
					const bool isAnimatedNode = animatedNodesSet.find(nodeName) != animatedNodesSet.end();
					if (isAnimatedNode)
						boneNames.insert(nodeName);

                    node = node->mParent;
                }
                while (node && node != meshNode && node != parentMeshNode);
            }
        }

        BuildSkeleton(parentMap, bones, boneNames, aiBones, scene->mRootNode, INVALID_BONE_INDEX);
        assert(bones.size() == boneNames.size());

		// if we dont have any animated bones, dont bother with bone weights etc
		if (animatedNodesSet.empty())
			return true;

		const bool processedOK = ProcessVertexBoneWeights(bones, meshes, meshNameMap, scene);

        return processedOK;
    }

    bool Assimp::ProcessVertexBoneWeights(std::vector<PackageBone>& bones, std::vector<PackageMesh>& meshes, const MeshNameMap& meshNameMap, const aiScene* scene)
    {
		for (PackageMesh& pkgMesh : meshes)
		{
			const std::string pkgMeshName = pkgMesh.mName;
			const aiMesh* assimpMesh = FindMesh(scene, meshNameMap, pkgMeshName);
			assert(assimpMesh);

			const uint32_t numVertices = assimpMesh->mNumVertices;
			pkgMesh.mBoneWeights.resize(numVertices);

			const uint32_t numBones = assimpMesh->mNumBones;
			assert(numBones <= MAX_NUM_BONES);

			for (uint32_t boneIndex = 0; boneIndex < assimpMesh->mNumBones; ++boneIndex)
			{
				const aiBone* assimpBone = assimpMesh->mBones[boneIndex];
				const BoneIndex bone = GetBoneIndex(meshes, bones, assimpBone->mName.C_Str());
				assert(bone != INVALID_BONE_INDEX);

				const uint32_t numWeights = assimpBone->mNumWeights;
				for (uint32_t weightIndex = 0; weightIndex < numWeights; ++weightIndex)
				{
					const auto assimpWeight = assimpBone->mWeights[weightIndex];
					const uint32_t vertexID = assimpWeight.mVertexId;
					const float weightFactor = assimpWeight.mWeight;

					auto& pkgWeight = pkgMesh.mBoneWeights.at(vertexID);
					const bool notExceededNumBones = UsedLessThanMaxNumBones(pkgWeight);
					assert(notExceededNumBones);

					uint32_t firstFreeIndex = 0;
					while (pkgWeight.mBoneIndices.at(firstFreeIndex) != INVALID_BONE_INDEX)
						++firstFreeIndex;

					pkgWeight.mBoneIndices.at(firstFreeIndex) = bone;
					pkgWeight.mBoneWeights.at(firstFreeIndex) = weightFactor;
				}
			}
		}

		return true;
    }

    bool Assimp::ProcessAnimations(PackageModel& model, const aiScene* scene)
    {
        if (!scene->HasAnimations())
            return true;

		uint32_t unnamedAnimationCounter = 0;
        for (uint32_t animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex)
        {
            aiAnimation* animation = *(scene->mAnimations + animationIndex);
			assert(animation);

            // what is this anyway... shouldn't be present
            assert(animation->mNumMeshChannels == 0);

			PackageAnimation& pkgAnimation = AddPkgAnimation(model, scene, animation, unnamedAnimationCounter);
			const auto animDuration = pkgAnimation.mDurationInMilliseconds;

            for (uint32_t nodeAnimIndex = 0; nodeAnimIndex < animation->mNumChannels; ++nodeAnimIndex)
            {
                aiNodeAnim* nodeAnimation = *(animation->mChannels + nodeAnimIndex);
				assert(nodeAnimation);

                // cant do scaling animations
                assert(nodeAnimation->mNumScalingKeys <= 1);

				const BoneIndex bone = GetBoneKeyframeContainer(pkgAnimation, model.mSkeleton, nodeAnimation);
				if (bone == INVALID_BONE_INDEX)
					continue;

				auto& keyFrames = pkgAnimation.mBoneAnimations.at(bone);

                const uint32_t numPosKeys = nodeAnimation->mNumPositionKeys;
                const uint32_t numRotkeys = nodeAnimation->mNumRotationKeys;
                const uint32_t maxNumKeys = glm::max(numPosKeys, numRotkeys);
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
                    uint32_t timestampMillisec = static_cast<uint32_t>(maxKeyTimeSeconds * 1000);
					// if this is the last frame and we are behind the total duration, drag this one out
					if (key == maxNumKeys - 1 && timestampMillisec < animDuration)
						timestampMillisec = animDuration;

					keyFrames.emplace_back(timestampMillisec, translateVector, rotationQuat);
                }
            }
        }

        return true;
    }


	bool ImportTexture(PackageTexture& texture, const boost::filesystem::path& modelPath, const std::string& texturePathStr, FreeImage& freeimageImporter)
	{
		const boost::filesystem::path texturePath = texturePathStr;

		std::string fullTexturePath = "";
		if (modelPath.has_parent_path())
		{
			fullTexturePath.append(modelPath.parent_path().string());
			fullTexturePath.append("/");
		}
		fullTexturePath.append(texturePath.string());

		return freeimageImporter.ProcessTexture(texture, fullTexturePath);
	}


    bool OnlyOneNodePerMesh(const aiScene* scene)
    {
		assert(scene);

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
		assert(scene);
		assert(node);

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
		assert(node);

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
    
    BoneIndex GetBoneIndex(const std::vector<PackageMesh>& meshes, const std::vector<PackageBone>& bones, const std::string& boneName)
    {
        for (const PackageMesh& mesh : meshes)
        {
            const uint32_t numBones = bones.size();
            for (BoneIndex boneIndex = 0; boneIndex < numBones; ++boneIndex)
            {
                const PackageBone& bone = bones.at(boneIndex);
                if (bone.mName == boneName)
                    return boneIndex;
            }
        }
        
        return INVALID_BONE_INDEX;
    }

    uint32_t CountChildren(const aiNode* node)
    {
		assert(node);

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
        for (uint32_t bone = 0; bone < MAX_NUM_BONES; ++bone)
        {
            // invalid index means unused
            if (pkgBoneWeight.mBoneIndices.at(bone) == INVALID_BONE_INDEX)
                return true;
        }
        
        return false;
    }

	template <typename T>
	bool DoesPkgNameExist(const std::vector<T>& pkgContainer, const std::string& name)
	{
		auto iter = std::find_if(pkgContainer.begin(), pkgContainer.end(), [&name](const T& item) { return item.mName == name; });

		return iter != pkgContainer.end();
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

	const aiMesh* FindMesh(const aiScene* scene, const MeshNameMap& meshNameMap, const std::string& pkgName)
	{
		assert(scene);

		auto iter = meshNameMap.find(pkgName);
		assert(iter != meshNameMap.end());
		const std::string& assimpName = iter->second;

		for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
		{
			const aiMesh* nodeMesh = scene->mMeshes[meshIndex];
			if (nodeMesh->mName.C_Str() == assimpName)
				return nodeMesh;
		}

		return nullptr;
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

    Vec3 GetVertices(const bool isStaticMesh, const Mat4& nodeTransform, const aiVector3D& assimpVertices)
    {
        Vec3 ret = aiVec3ToJonsVec3(assimpVertices);
        if (isStaticMesh)
            ret = Vec3(nodeTransform * Vec4(ret, 1.0f));

        return ret;
    }
    

	PackageAnimation& AddPkgAnimation(PackageModel& model, const aiScene* scene, const aiAnimation* animation, uint32_t& unnamedAnimationCounter)
	{
		assert(animation);

		std::string animName = animation->mName.C_Str();
	    bool animNameFound = DoesPkgNameExist<PackageAnimation>(model.mAnimations, animName);
		if (animNameFound)
			Log("WARNING: Name collision for animation " + animName);

		// name collision - need to rename material
		while (animName.empty() || animNameFound)
		{
			animName = "Animation_" + std::to_string(unnamedAnimationCounter++);
			animNameFound = DoesPkgNameExist<PackageAnimation>(model.mAnimations, animName);
		}

        // scene root node is always first
		const Mat4& rootNodeTransform = model.mNodes.front().mTransform;
		const Mat4 invRootNodeTransform = glm::inverse(rootNodeTransform);

		const uint32_t durationMillisec = static_cast<uint32_t>((animation->mDuration / animation->mTicksPerSecond) * 1000);
		model.mAnimations.emplace_back(animName, durationMillisec, invRootNodeTransform);
		PackageAnimation& pkgAnimation = model.mAnimations.back();

		// number of nodeanim channels should be same as num bones
		// TODO: vector of vectors - rearrange boneanims for better cache utilization?
		const uint32_t numBones = model.mSkeleton.size();
		pkgAnimation.mBoneAnimations.resize(numBones);

		return pkgAnimation;
	}

	BoneIndex GetBoneKeyframeContainer(PackageAnimation& pkgAnimation, const std::vector<PackageBone>& bones, const aiNodeAnim* nodeAnimation)
	{
		const std::string nodeAnimName = nodeAnimation->mNodeName.C_Str();
		const auto boneBeginIter = bones.begin();
		const auto boneEndIter = bones.end();
		const auto boneIter = std::find_if(boneBeginIter, boneEndIter, [&nodeAnimName](const PackageBone& pkgBone) { return pkgBone.mName == nodeAnimName; });
		if (boneIter == boneEndIter)
			return INVALID_BONE_INDEX;

		return boneIter - boneBeginIter;
	}

	void BuildSkeleton(BoneParentMap& parentMap, std::vector<PackageBone>& skeleton, const BoneNameSet& boneNames, const AssimpBoneSet& aiBones, const aiNode* node, const BoneIndex parentBone)
	{
		const std::size_t numBones = boneNames.size();
		parentMap.resize(numBones, INVALID_BONE_INDEX);
		skeleton.reserve(numBones);

		BoneIndex thisBone = INVALID_BONE_INDEX;
		const auto boneNameIter = boneNames.find(node->mName.C_Str());
		if (boneNameIter != boneNames.end())
		{
			const std::string& boneName = *boneNameIter;
			const aiBone* bone = FindAiBoneByName(aiBones, boneName);

			if (bone)
                skeleton.emplace_back(boneName, aiMat4ToJonsMat4(bone->mOffsetMatrix));
            else
                //skeleton.emplace_back(boneName, glm::inverse(nodeTransform));
                skeleton.emplace_back(boneName, gIdentityMatrix);

			thisBone = static_cast<BoneIndex>(skeleton.size() - 1);
			parentMap.at(thisBone) = parentBone;

			// parents must always appear infront of children to speed up updating transforms
			assert(parentBone == INVALID_BONE_INDEX || parentBone < thisBone);
		}

		for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
		{
			const aiNode* childNode = node->mChildren[childIndex];
			BuildSkeleton(parentMap, skeleton, boneNames, aiBones, childNode, thisBone);
		}
	}

	std::set<std::string> GetListOfAnimatedNodes(const aiScene* scene)
	{
		std::set<std::string> ret;

		for (uint32_t animationIndex = 0; animationIndex < scene->mNumAnimations; ++animationIndex)
		{
			const aiAnimation* animation = scene->mAnimations[animationIndex];
			assert(animation);
			for (uint32_t nodeAnimIndex = 0; nodeAnimIndex < animation->mNumChannels; ++nodeAnimIndex)
			{
				const aiNodeAnim* nodeAnim = animation->mChannels[nodeAnimIndex];
				assert(nodeAnim);

				ret.insert(nodeAnim->mNodeName.C_Str());
			}
		}

		return ret;
	}
    
	// animated model: needs to transform all nodes using all the animations to find the maximum extents
	// results in a looser, static aabb atleast the size of the aabb in bind pose
	// basically implements CPU skinning to evaluate max aabb...
    void ParseStaticAABBForAnimatedModel(PackageModel& model)
    {
        if (model.mAnimations.empty())
            return;
    
		const Milliseconds keyTickrate = Milliseconds(16);
		auto& parentMap = model.mBoneParentMap;
		BoneTransforms boneOffsets;
		for (const auto& pkgBone : model.mSkeleton)
			boneOffsets.emplace_back(pkgBone.mOffsetMatrix);

		const std::size_t numBones = model.mSkeleton.size();
		BoneTransforms boneTransforms(numBones, gIdentityMatrix);
		for (const PackageAnimation& pkgAnimation : model.mAnimations)
		{
			// use/feed the engine animation class to help interpolating etc
			Animation jonsAnimation(pkgAnimation, parentMap, boneOffsets);

			const Milliseconds animDuration(pkgAnimation.mDurationInMilliseconds);
			Milliseconds currTime(0);
			while (currTime < animDuration)
			{
				UpdateBoneTransforms(currTime, jonsAnimation, boneTransforms, numBones);
				TransformsVerticesAndCheckAABB(model, boneTransforms, model.mStaticAABB.mMinBounds, model.mStaticAABB.mMaxBounds);

				currTime += keyTickrate;
			}
		}
    }

	void UpdateBoneTransforms(const Milliseconds currTimestamp, const Animation& jonsAnimation, BoneTransforms& boneTransforms, const std::size_t numBones)
	{
		const bool calcUsingRepeatAnimation = true;

		Mat4& rootTransform = boneTransforms.front();
		rootTransform = jonsAnimation.InterpolateBoneTransform(0, currTimestamp, calcUsingRepeatAnimation);
		for (BoneIndex bone = 1; bone < numBones; ++bone)
		{
			const BoneIndex parentInstance = jonsAnimation.GetParentIndex(bone);
			const Mat4& parentTransform = boneTransforms.at(parentInstance);

			Mat4& transform = boneTransforms.at(bone);
			transform = parentTransform * jonsAnimation.InterpolateBoneTransform(bone, currTimestamp, calcUsingRepeatAnimation);
		}

		for (BoneIndex bone = 1; bone < numBones; ++bone)
		{
			const Mat4& boneOffsetTransform = jonsAnimation.GetBoneOffsetTransform(bone);
			Mat4& transform = boneTransforms.at(bone);
			transform = transform * boneOffsetTransform;
		}
	}

	void TransformsVerticesAndCheckAABB(PackageModel& model, const BoneTransforms& boneTransforms, Vec3& minExtent, Vec3& maxExtent)
	{
		const std::size_t vertexStepSize = 3;

		for (PackageNode& node : model.mNodes)
		{
			const std::size_t numMeshes = node.mMeshes.size();
			for (PackageMesh::MeshIndex meshIndex = 0; meshIndex < numMeshes; ++meshIndex)
			{
				PackageMesh& mesh = model.mMeshes.at(meshIndex);
				const std::size_t numVertices = mesh.mVertexData.size() / vertexStepSize;
				for (std::size_t vertexNum = 0; vertexNum < numVertices; ++vertexNum)
				{
					std::size_t vertexPos = vertexNum * vertexStepSize;
					// x = 0, y = +1, z = +2
					Vec4 vertex(mesh.mVertexData.at(vertexPos), mesh.mVertexData.at(vertexPos + 1), mesh.mVertexData.at(vertexPos + 2), 1.0f);
					const BoneWeight& weights = mesh.mBoneWeights.at(vertexNum);

					uint32_t weightNum = 0;
					Mat4 boneTransform(node.mTransform);
					while (weightNum < MAX_BONES_PER_VERTEX)
					{
						const BoneIndex bone = weights.mBoneIndices.at(weightNum);
						const float weight = weights.mBoneWeights.at(weightNum);
						if (bone == INVALID_BONE_INDEX)
							break;

						const Mat4& transform = boneTransforms.at(bone);
						boneTransform = boneTransform * (transform * weight);

						++weightNum;
					}

					const Vec3 transformedVertex = Vec3(boneTransform * vertex);
					mesh.mAABB.mMinBounds = MinVal(mesh.mAABB.mMinBounds, transformedVertex);
					mesh.mAABB.mMaxBounds = MaxVal(mesh.mAABB.mMaxBounds, transformedVertex);
				}
			}
		}
		
		for (const PackageMesh& mesh : model.mMeshes)
		{
			minExtent = MinVal(minExtent, mesh.mAABB.mMinBounds);
			maxExtent = MaxVal(maxExtent, mesh.mAABB.mMaxBounds);
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
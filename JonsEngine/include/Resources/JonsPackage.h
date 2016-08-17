#pragma once

#include "include/Core/Types.h"
#include "include/Core/Utils/Time.h"
#include "include/Resources/Bone.h"
#include "include/Resources/BoneKeyframe.h"

#include <array>
#include <vector>
#include <memory>

// disable untill boost::serialization fixes it
#pragma warning(disable : 4244 4308)

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

namespace JonsEngine
{
    // TODO: currently a mix of SoA and AoS - refactor into AoS for better performance?

    const uint8_t LatestMajorVersion = 0;
    const uint8_t LatestMinorVersion = 1;

    struct PackageHeader
    {
        PackageHeader();
    
    
        std::string mSignature;
        uint8_t mMajorVersion;
        uint8_t mMinorVersion;
    };

    struct PackageAABB
    {
        PackageAABB();
        
    
        Vec3 mMinBounds;
        Vec3 mMaxBounds;
    };

    struct PackageTexture
    {
        PackageTexture();
    
    
        std::vector<uint8_t> mTextureData;
        uint32_t mTextureWidth;         // width/height in pixels
        uint32_t mTextureHeight;
    };

    struct PackageSkybox
    {
        PackageSkybox();
        PackageSkybox(const std::string& name);
        
    
        std::string mName;
        PackageTexture mSkyboxTexture;
    };

    struct PackageMaterial
    {
        typedef uint32_t MaterialIndex;
        static const MaterialIndex INVALID_MATERIAL_INDEX = UINT32_MAX;

        PackageMaterial();
        PackageMaterial(const std::string& name, const bool hasDiffTexture, const bool hasNormalTexture);
        
        
        std::string mName;
        bool mHasDiffuseTexture;
        bool mHasNormalTexture;
        PackageTexture mDiffuseTexture;
        PackageTexture mNormalTexture;
        Vec3 mDiffuseColor;
        Vec3 mAmbientColor;
        Vec3 mSpecularColor;
        Vec3 mEmissiveColor;
    };

    struct PackageBone
    {
        PackageBone();
        PackageBone(const std::string& name, const Mat4& offsetMatrix);
        

        std::string mName;
        Mat4 mOffsetMatrix;
    };

	struct PackageBoneKeyframe
	{
		PackageBoneKeyframe();
		PackageBoneKeyframe(const uint32_t timestampMillisec, const Vec3& translation, const Quaternion& rotation);


		uint32_t mTimestampMillisec;
		Vec3 mTranslation;
		Quaternion mRotation;
	};

    struct PackageMesh
    {
        typedef uint32_t MeshIndex;
        static const MeshIndex INVALID_MESH_INDEX = UINT32_MAX;
        
        PackageMesh();
        PackageMesh(const std::string& name);
        

        std::string mName;
        PackageAABB mAABB;
        std::vector<float> mVertexData;
        std::vector<float> mNormalData;
        std::vector<float> mTexCoordsData;
        std::vector<float> mTangentData;
        std::vector<BoneWeight> mBoneWeights;
        std::vector<uint16_t> mIndiceData;
        PackageMaterial::MaterialIndex mMaterialIndex;
    };

    struct PackageNode
    {
        typedef uint32_t NodeIndex;
        static const NodeIndex INVALID_NODE_INDEX = UINT32_MAX;
        
        PackageNode();
        PackageNode(const std::string& name, const Mat4& transform, const NodeIndex nodeIndex, const NodeIndex parentNodeIndex);
        

        std::string mName;
		Mat4 mTransform;
        NodeIndex mNodeIndex;
        NodeIndex mParentNodeIndex;
        std::vector<PackageMesh::MeshIndex> mMeshes;
    };

    struct PackageAnimation
    {
		typedef std::vector<PackageBoneKeyframe> KeyFrameContainer;
		typedef std::vector<KeyFrameContainer> BoneAnimationContainer;

        PackageAnimation();
        PackageAnimation(const std::string& name, const uint32_t durationMilliseconds, const Mat4& invRootMatrix);
    
    
        std::string mName;
		uint32_t mDurationInMilliseconds;
        Mat4 mInverseRootMatrix;
		BoneAnimationContainer mBoneAnimations;
    };

    struct PackageModel
    {
        PackageModel();
        PackageModel(const std::string& modelName);
        
    
        std::string mName;
        PackageAABB mStaticAABB;
        std::vector<PackageMesh> mMeshes;
        std::vector<PackageNode> mNodes;
        std::vector<PackageBone> mSkeleton;
        std::vector<PackageAnimation> mAnimations;
		BoneParentMap mBoneParentMap;
    };

    struct JonsPackage
    {
        PackageHeader mHeader;
        std::vector<PackageModel> mModels;
        std::vector<PackageMaterial> mMaterials;
        std::vector<PackageSkybox> mSkyBoxes;
    };


    typedef std::shared_ptr<JonsPackage> JonsPackagePtr;


    /*
     * Reading/writing JonsPackage files
     */
    JonsPackagePtr ReadJonsPkg(const std::string& jonsPkgName);
    bool WriteJonsPkg(const std::string& jonsPkgName, const JonsPackagePtr jonsPkg);
}


namespace boost
{
    namespace serialization
    {
        /* Defines how to serialize JonsPackage */
        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageHeader& header, const unsigned int version)
        {
            ar & header.mMajorVersion;
            ar & header.mMinorVersion;
            ar & header.mSignature;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageAABB& aabb, const unsigned int version)
        {
            ar & aabb.mMinBounds;
            ar & aabb.mMaxBounds;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageBone& bone, const unsigned int version)
        {
            ar & bone.mName;
            ar & bone.mOffsetMatrix;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageMesh& mesh, const unsigned int version)
        {
            ar & mesh.mName;
            ar & mesh.mAABB;
            ar & mesh.mVertexData;
            ar & mesh.mNormalData;
            ar & mesh.mTexCoordsData;
            ar & mesh.mTangentData;
            ar & mesh.mBoneWeights;
            ar & mesh.mIndiceData;
            ar & mesh.mMaterialIndex;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageNode& node, const unsigned int version)
        {
            ar & node.mName;
			ar & node.mTransform;
            ar & node.mNodeIndex;
            ar & node.mParentNodeIndex;
            ar & node.mMeshes;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageTexture& texture, const unsigned int version)
        {
            ar & texture.mTextureData;
            ar & texture.mTextureWidth;
            ar & texture.mTextureHeight;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageSkybox& skybox, const unsigned int version)
        {
            ar & skybox.mName;
            ar & skybox.mSkyboxTexture;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageMaterial& material, const unsigned int version)
        {
            ar & material.mName;
            ar & material.mHasDiffuseTexture;
            ar & material.mHasNormalTexture;
            ar & material.mDiffuseTexture;
            ar & material.mNormalTexture;
            ar & material.mDiffuseColor;
            ar & material.mAmbientColor;
            ar & material.mSpecularColor;
            ar & material.mEmissiveColor;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageBoneKeyframe& keyframe, const unsigned int version)
        {
            ar & keyframe.mTimestampMillisec;
            ar & keyframe.mTranslation;
            ar & keyframe.mRotation;
        }

		template<class Archive>
		void serialize(Archive & ar, JonsEngine::BoneWeight& boneWeight, const unsigned int version)
		{
			ar & boneWeight.mBoneIndices;
			ar & boneWeight.mBoneWeights;
		}
        
        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageAnimation& animation, const unsigned int version)
        {
            ar & animation.mName;
			ar & animation.mDurationInMilliseconds;
            ar & animation.mInverseRootMatrix;
            ar & animation.mBoneAnimations;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageModel& model, const unsigned int version)
        {
            ar & model.mName;
            ar & model.mStaticAABB;
            ar & model.mMeshes;
            ar & model.mNodes;
            ar & model.mSkeleton;
            ar & model.mAnimations;
			ar & model.mBoneParentMap;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::JonsPackage& pkg, const unsigned int version)
        {
            ar & pkg.mHeader;
            ar & pkg.mModels;
            ar & pkg.mMaterials;
            ar & pkg.mSkyBoxes;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::Mat4& transform, const unsigned int version)
        {
            ar & transform[0];
            ar & transform[1];
            ar & transform[2];
            ar & transform[3];
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::Vec4& vec, const unsigned int version)
        {
            ar & vec.x;
            ar & vec.y;
            ar & vec.z;
            ar & vec.w;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::Vec3& vec, const unsigned int version)
        {
            ar & vec.x;
            ar & vec.y;
            ar & vec.z;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::Vec2& vec, const unsigned int version)
        {
            ar & vec.x;
            ar & vec.y;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::Quaternion& quat, const unsigned int version)
        {
            ar & quat.x;
            ar & quat.y;
            ar & quat.z;
            ar & quat.w;
        }
    } // namespace serialization
} // namespace boost
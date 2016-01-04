#pragma once

#include "include/Core/Types.h"
#include "include/Core/Utils/Time.h"

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
    const uint8_t LatestMajorVersion = 0;
    const uint8_t LatestMinorVersion = 1;

    struct PackageHeader
    {
        std::string mSignature;
        uint8_t mMajorVersion;
        uint8_t mMinorVersion;


        PackageHeader();
    };

    struct PackageAABB
    {
        Vec3 mMinBounds;
        Vec3 mMaxBounds;


        PackageAABB();
    };

    struct PackageTexture
    {
        std::vector<uint8_t> mTextureData;
        uint32_t mTextureWidth;         // width/height in pixels
        uint32_t mTextureHeight;


        PackageTexture();
    };

    struct PackageSkybox
    {
        std::string mName;
        PackageTexture mSkyboxTexture;


        PackageSkybox();
        PackageSkybox(const std::string& name);
    };

    struct PackageMaterial
    {
        typedef uint32_t MaterialIndex;
        static const MaterialIndex INVALID_MATERIAL_INDEX = UINT32_MAX;

        std::string mName;
        bool mHasDiffuseTexture;
        bool mHasNormalTexture;
        PackageTexture mDiffuseTexture;
        PackageTexture mNormalTexture;
        Vec3 mDiffuseColor;
        Vec3 mAmbientColor;
        Vec3 mSpecularColor;
        Vec3 mEmissiveColor;


        PackageMaterial();
        PackageMaterial(const std::string& name, const bool hasDiffTexture, const bool hasNormalTexture);
    };

    struct PackageBone
    {
        std::string mName;
        Mat4 mTransform;


        PackageBone();
    };

    struct PackageMesh
    {
        typedef uint32_t MeshIndex;
        static const MeshIndex INVALID_MESH_INDEX = UINT32_MAX;

        std::string mName;
        PackageAABB mAABB;
        std::vector<PackageBone> mBones;
        std::vector<float> mVertexData;
        std::vector<float> mNormalData;
        std::vector<float> mTexCoordsData;
        std::vector<float> mTangentData;
        std::vector<uint16_t> mIndiceData;
        PackageMaterial::MaterialIndex mMaterialIndex;


        PackageMesh();
        PackageMesh(const std::string& name);
    };

    struct PackageNode
    {
        typedef uint32_t NodeIndex;
        static const NodeIndex INVALID_NODE_INDEX = UINT32_MAX;

        std::string mName;
        NodeIndex mNodeIndex;
        NodeIndex mParentNodeIndex;
        PackageAABB mAABB;
        std::vector<PackageMesh::MeshIndex> mMeshes;


        PackageNode();
        PackageNode(const std::string& name, const NodeIndex nodeIndex, const NodeIndex parentNodeIndex);
    };

    struct PackageAnimatedNodeTransform
    {
        uint32_t mTimestampMilliseconds;
        Mat4 mTransform;


        PackageAnimatedNodeTransform();
        PackageAnimatedNodeTransform(const uint32_t timestampMilliseconds, const Mat4& transform);
    };

    struct PackageAnimatedNode
    {
        PackageNode::NodeIndex mNodeIndex;
        std::vector<PackageAnimatedNodeTransform> mAnimationTransforms;


        PackageAnimatedNode();
        PackageAnimatedNode(const PackageNode::NodeIndex nodeIndex);
    };

    struct PackageAnimation
    {
        std::string mName;
        uint32_t mDurationInMilliseconds;
        std::vector<PackageAnimatedNode> mAnimatedNodes;


        PackageAnimation();
        PackageAnimation(const std::string& name, const uint32_t durationMilliseconds);
    };

    struct PackageModel
    {
        std::string mName;
        PackageAABB mStaticAABB;
        std::vector<PackageMesh> mMeshes;
        std::vector<PackageNode> mNodes;
        std::vector<PackageAnimation> mAnimations;


        PackageModel();
        PackageModel(const std::string& modelName);
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
        void serialize(Archive & ar, JonsEngine::PackageMesh& mesh, const unsigned int version)
        {
            ar & mesh.mName;
            ar & mesh.mAABB;

            ar & mesh.mVertexData;
            ar & mesh.mNormalData;
            ar & mesh.mTexCoordsData;
            ar & mesh.mTangentData;
            ar & mesh.mIndiceData;
            ar & mesh.mMaterialIndex;
            ar & mesh.mHasMaterial;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageNode& node, const unsigned int version)
        {
            ar & node.mName;
            ar & node.mNodeIndex;
            ar & node.mParentNodeIndex;
            ar & node.mAABB;
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
        void serialize(Archive & ar, JonsEngine::PackageAnimatedNodeTransform& animationNodeTransform, const unsigned int version)
        {
            ar & animationNodeTransform.mTimestampMilliseconds;
            ar & animationNodeTransform.mTransform;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageAnimatedNode& animationNode, const unsigned int version)
        {
            ar & animationNode.mNodeIndex;
            ar & animationNode.mAnimationTransforms;
        }
        
        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageAnimation& animation, const unsigned int version)
        {
            ar & animation.mName;
            ar & animation.mDurationInMilliseconds;
            ar & animation.mAnimatedNodes;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageModel& model, const unsigned int version)
        {
            ar & model.mName;
            ar & model.mStaticAABB;
            ar & model.mMeshes;
            ar & model.mNodes;
            ar & model.mAnimations;
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
    } // namespace serialization
} // namespace boost
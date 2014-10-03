#pragma once

#include "include/Core/Types.h"
#include "include/Core/EngineDefs.h"

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

    struct PackageMesh
    {
        std::vector<float> mVertexData;
        std::vector<float> mNormalData;
        std::vector<float> mTexCoordsData;
        std::vector<float> mTangents;
        std::vector<float> mBitangents;
        std::vector<uint32_t> mIndiceData;
        uint16_t mMaterialIndex;
        bool mHasMaterial;


        PackageMesh();
    };

    struct PackageTexture
    {
        std::string mName;
        std::vector<uint8_t> mTextureData;
        uint32_t mTextureWidth;         // width/height in pixels
        uint32_t mTextureHeight;
        TextureType mTextureType;


        PackageTexture();
    };

    struct PackageMaterial
    {
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
    };

    struct PackageModel
    {
        std::string mName;
        std::vector<PackageModel> mChildren;
        std::vector<PackageMesh> mMeshes;
        Mat4 mTransform;


        PackageModel();
    };

    struct JonsPackage
    {
        PackageHeader mHeader;
        std::vector<PackageModel> mModels;
        std::vector<PackageMaterial> mMaterials;


        JonsPackage();
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
        void serialize(Archive & ar, JonsEngine::PackageModel& model, const unsigned int version)
        {
            ar & model.mName;
            ar & model.mChildren;
            ar & model.mMeshes;
            ar & model.mTransform;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageMesh& mesh, const unsigned int version)
        {
            ar & mesh.mVertexData;
            ar & mesh.mNormalData;
            ar & mesh.mTexCoordsData;
            ar & mesh.mTangents;
            ar & mesh.mBitangents;
            ar & mesh.mIndiceData;
            ar & mesh.mMaterialIndex;
            ar & mesh.mHasMaterial;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageTexture& texture, const unsigned int version)
        {
            ar & texture.mName;
            ar & texture.mTextureData;
            ar & texture.mTextureWidth;
            ar & texture.mTextureHeight;
            ar & texture.mTextureType;
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
        void serialize(Archive & ar, JonsEngine::JonsPackage& pkg, const unsigned int version)
        {
            ar & pkg.mHeader;
            ar & pkg.mModels;
            ar & pkg.mMaterials;
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
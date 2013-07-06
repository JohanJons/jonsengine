#pragma once

#include "include/Core/Types.h"
#include "include/Core/EngineDefs.h"
#include "include/Renderer/ITexture.h"

#include <vector>
#include "boost/smart_ptr.hpp"

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

    /* PackageHeader definition */
    struct PackageHeader
    {
        std::string mSignature;
        uint8_t mMajorVersion;
        uint8_t mMinorVersion;


        PackageHeader();
    };

    /* PackageMesh definition */
    struct PackageMesh
    {
        std::vector<float> mVertexData;
        std::vector<float> mNormalData;
        std::vector<float> mTexCoordsData;
        std::vector<uint32_t> mIndiceData;
        uint16_t mMaterialIndex;
        bool mHasMaterial;


        PackageMesh();
    };

    /* PackageTexture definition */
    struct PackageTexture
    {
        std::string mName;
        std::vector<uint8_t> mTextureData;
        uint32_t mTextureWidth;         // width/height in pixels
        uint32_t mTextureHeight;
        ITexture::TextureFormat mTextureFormat;
        ITexture::TextureType mTextureType;


        PackageTexture();
    };

    /* PackageMaterial definition */
    struct PackageMaterial
    {
        std::string mName;
        PackageTexture mDiffuseTexture;
        Vec3 mDiffuseColor;
        Vec3 mAmbientColor;
        Vec3 mSpecularColor;
        Vec3 mEmissiveColor;


        PackageMaterial();
    };

    /* PackageModel definition */
    struct PackageModel
    {
        std::string mName;
        std::vector<PackageModel> mChildren;
        std::vector<PackageMesh> mMeshes;
        Mat4 mTransform;


        PackageModel();
    };

    /* JonsPackage definition */
    struct JonsPackage
    {
        PackageHeader mHeader;
        std::vector<PackageModel> mModels;
        std::vector<PackageMaterial> mMaterials;


        JonsPackage();
    };

    /* JonsPackagePtr definition */
    typedef boost::shared_ptr<JonsPackage> JonsPackagePtr;


    /*
     * Reading/writing JonsPackage files
     */
    JonsPackagePtr ReadJonsPkg(const std::string& jonsPkgName);
    bool WriteJonsPkg(const std::string& jonsPkgName, const JonsPackagePtr pkg);


    /* PackageHeader inlines */
    inline PackageHeader::PackageHeader() : mSignature("jons"), mMajorVersion(LatestMajorVersion), mMinorVersion(LatestMinorVersion)
    {
    }

    /* PackageModel inlines */
    inline PackageModel::PackageModel() : mName(""), mTransform(1.0f)
    {
    }

    /* PackageMesh inlines */
    inline PackageMesh::PackageMesh() : mMaterialIndex(0), mHasMaterial(false)
    {
    }

    /* PackageTexture inlines */
    inline PackageTexture::PackageTexture() : mName(""), mTextureWidth(0), mTextureHeight(0), mTextureFormat(ITexture::UNKNOWN_FORMAT), mTextureType(ITexture::UNKNOWN_TYPE)
    {
    }

    /* PackageMaterial inlines */
    inline PackageMaterial::PackageMaterial() : mName(""), mDiffuseColor(0.0f), mAmbientColor(0.0f), mSpecularColor(0.0f), mEmissiveColor(0.0f)
    {
    }

    /* JonsPackage inlines */
    inline JonsPackage::JonsPackage()
    {
    }
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
            ar & texture.mTextureFormat;
            ar & texture.mTextureType;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::PackageMaterial& material, const unsigned int version)
        {
            ar & material.mName;
            ar & material.mDiffuseTexture;
            ar & material.mDiffuseColor;
            ar & material.mAmbientColor;
            ar & material.mSpecularColor;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::JonsPackage& pkg, const unsigned int version)
        {
            ar & pkg.mHeader;
            ar & pkg.mModels;
            ar & pkg.mMaterials;
        }

        template<class Archive>
        void serialize(Archive & ar, glm::detail::tmat4x4<glm::mediump_float>& transform, const unsigned int version)
        {
            ar & transform[0];
            ar & transform[1];
            ar & transform[2];
            ar & transform[3];
        }

        template<class Archive>
        void serialize(Archive & ar, glm::detail::tvec4<glm::mediump_float>& vec, const unsigned int version)
        {
            ar & vec.x;
            ar & vec.y;
            ar & vec.z;
            ar & vec.w;
        }

        template<class Archive>
        void serialize(Archive & ar, glm::detail::tvec3<glm::mediump_float>& vec, const unsigned int version)
        {
            ar & vec.x;
            ar & vec.y;
            ar & vec.z;
        }

        template<class Archive>
        void serialize(Archive & ar, glm::detail::tvec2<glm::mediump_float>& vec, const unsigned int version)
        {
            ar & vec.x;
            ar & vec.y;
        }
    } // namespace serialization
} // namespace boost
#pragma once

#include "include/Core/Types.h"
#include "include/Core/EngineDefs.h"

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
        std::vector<uint32_t> mIndiceData;     // TODO: evaluate if uint32_t byte is really needed, otherwise go with uint16_t to reduce size...

        
        PackageMesh();
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
    inline PackageMesh::PackageMesh()
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
            ar & mesh.mIndiceData;
        }

        template<class Archive>
        void serialize(Archive & ar, JonsEngine::JonsPackage& pkg, const unsigned int version)
        {
            ar & pkg.mHeader;
            ar & pkg.mModels;
        }

        template<class Archive>
        void serialize(Archive & ar, glm::detail::tmat4x4<glm::mediump_float> transform, const unsigned int version)
        {
            ar & transform[0];
            ar & transform[1];
            ar & transform[2];
            ar & transform[3];
        }

        template<class Archive>
        void serialize(Archive & ar, glm::detail::tvec4<glm::mediump_float> vec, const unsigned int version)
        {
            ar & vec.x;
            ar & vec.y;
            ar & vec.z;
            ar & vec.w;
        }
    } // namespace serialization
} // namespace boost
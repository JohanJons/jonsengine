#pragma once

#include "include/Core/Types.h"

#include <string>
#include <memory>

namespace JonsEngine
{
    class Skybox
    {
    public:
        Skybox(const std::string& name, const MaterialID skyboxTexture);

        bool operator==(const Skybox& m);
        bool operator==(const std::string& modelName);


        const std::string mName;
        const MaterialID mSkyboxTexture;


    private:
        const size_t mHashedID;
    };

    typedef std::shared_ptr<Skybox> SkyboxPtr;
}
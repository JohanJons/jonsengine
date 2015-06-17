#pragma once

#include "include/Core/Types.h"

#include <string>
#include <memory>

namespace JonsEngine
{
    class Skybox
    {
    public:
        Skybox(const std::string& name, const TextureID skyboxTexture);

        bool operator==(const Skybox& m);
        bool operator==(const std::string& modelName);


        const std::string mName;
        const TextureID mSkyboxTexture;


    private:
        const size_t mHashedID;
    };

    typedef std::shared_ptr<Skybox> SkyboxPtr;
}
#include "include/Resources/Skybox.h"

namespace JonsEngine
{
    Skybox::Skybox(const std::string& name, const DX11TextureID skyboxTexture) : mName(name), mSkyboxTexture(skyboxTexture)
    {
    }

    Skybox::~Skybox()
    {
    }


    const std::string& Skybox::GetName() const
    {
        return mName;
    }

    DX11TextureID Skybox::GetSkyboxTexture() const
    {
        return mSkyboxTexture;
    }
}
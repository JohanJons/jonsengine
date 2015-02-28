#pragma once

#include "include/Core/Types.h"
#include "include/Scene/SceneNode.h"

#include <string>
#include <memory>
#include <functional>

namespace JonsEngine
{
    class PointLight;

    typedef std::function<void(PointLight*, SceneNode*)> OnChangeSceneNodeFunc;
    typedef std::function<void(PointLight*)> OnDirtyFunc;

    class PointLight
    {
    public:
        PointLight(const std::string& name, const OnChangeSceneNodeFunc& onChangeSceneNode, const OnDirtyFunc& onDirty);

        bool operator==(const PointLight& light);
        bool operator==(const std::string& lightName);

        void SetSceneNode(SceneNodePtr sceneNode);
        SceneNodePtr GetSceneNode();

        void SetLightColor(const Vec4& color);
        const Vec4& GetLightColor() const;

        void SetLightIntensity(const float intensity);
        float GetLightIntensity() const;

        void SetLightRadius(const float radius);
        float GetLightRadius() const;

		
        const std::string mName;


	private:
		const size_t mHashedID;

        SceneNodePtr mSceneNode;
        Vec4 mLightColor;
        float mLightIntensity;
        float mLightRadius;

        const OnChangeSceneNodeFunc mOnChangeSceneNode;
        const OnDirtyFunc mOnDirty;
    };

	typedef std::unique_ptr<PointLight, std::function<void(PointLight*)>> PointLightPtr;
}
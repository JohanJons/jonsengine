#pragma once

#include "include/Scene/SceneNode.h"
#include "include/Scene/Model.h"

#include <memory>
#include <functional>

namespace JonsEngine
{
    class Actor
    {
    public:
        Actor(const std::string& name, const ModelPtr model, const SceneNodePtr node);

		bool operator==(const Actor& actor);
        bool operator==(const std::string& actorName);


        const std::string mName;

        ModelPtr mModel;
        SceneNodePtr mSceneNode;


	private:
		const size_t mHashedID;
    };

    typedef std::unique_ptr<Actor, std::function<void(Actor*)>> ActorPtr;
}
#include "SceneTest.h"

#include "include/Scene/Scene.h"

#include <string>

namespace JonsEngine
{
    /**
	 * GetSceneName/GetRootNode test
	 */
	TEST_F(SceneTest, CreateDeleteScene)
	{
        const std::string sceneName = "TestScene";

        Scene* scene = mEngine->GetSceneManager().CreateScene(sceneName);
        
        ASSERT_EQ(scene->GetSceneName(), sceneName);
        ASSERT_EQ(scene->GetRootNode().GetNodeName(), "Root");

        mEngine->GetSceneManager().DeleteScene(scene);
	}

}
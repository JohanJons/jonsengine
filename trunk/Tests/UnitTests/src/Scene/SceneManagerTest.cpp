#include "SceneManagerTest.h"

#include "include/Scene/SceneManager.h"
#include "include/Scene/Scene.h"

#include <string>

namespace JonsEngine
{
    /**
	 * CreateScene/DeleteScene test
	 */
	TEST_F(SceneManagerTest, CreateDeleteScene)
	{
		const std::string sceneName = "TestScene";

        ASSERT_EQ((int)mEngine->GetSceneManager().FindScene(sceneName), NULL);

        Scene* scene = mEngine->GetSceneManager().CreateScene(sceneName);
        ASSERT_NE((int)mEngine->GetSceneManager().FindScene(sceneName), NULL);

        scene = mEngine->GetSceneManager().CreateScene(sceneName);
        ASSERT_EQ((int)scene, NULL);

        mEngine->GetSceneManager().DeleteScene(sceneName);
        ASSERT_EQ((int)mEngine->GetSceneManager().FindScene(sceneName), NULL);

        scene = mEngine->GetSceneManager().CreateScene(sceneName);
        ASSERT_NE((int)mEngine->GetSceneManager().FindScene(sceneName), NULL);

        mEngine->GetSceneManager().DeleteScene(scene);
        ASSERT_EQ((int)mEngine->GetSceneManager().FindScene(sceneName), NULL);
	}

    /**
	 * FindScene test
	 */
	TEST_F(SceneManagerTest, FindScene)
	{
		const std::string sceneName1 = "TestScene1";
        const std::string sceneName2 = "TestScene2";
        const std::string sceneName3 = "TestScene3";
        const std::string sceneName4 = "NotCreated";

        ASSERT_EQ((int)mEngine->GetSceneManager().FindScene(sceneName1), NULL);
        ASSERT_EQ((int)mEngine->GetSceneManager().FindScene(sceneName2), NULL);
        ASSERT_EQ((int)mEngine->GetSceneManager().FindScene(sceneName3), NULL);

        mEngine->GetSceneManager().CreateScene(sceneName1);
        mEngine->GetSceneManager().CreateScene(sceneName2);
        mEngine->GetSceneManager().CreateScene(sceneName3);

        ASSERT_NE((int)mEngine->GetSceneManager().FindScene(sceneName1), NULL);
        ASSERT_NE((int)mEngine->GetSceneManager().FindScene(sceneName2), NULL);
        ASSERT_NE((int)mEngine->GetSceneManager().FindScene(sceneName3), NULL);

        mEngine->GetSceneManager().DeleteScene(sceneName1);
        mEngine->GetSceneManager().DeleteScene(sceneName2);
        mEngine->GetSceneManager().DeleteScene(sceneName3);

        ASSERT_EQ((int)mEngine->GetSceneManager().FindScene(sceneName1), NULL);
        ASSERT_EQ((int)mEngine->GetSceneManager().FindScene(sceneName2), NULL);
        ASSERT_EQ((int)mEngine->GetSceneManager().FindScene(sceneName3), NULL);

        ASSERT_EQ((int)mEngine->GetSceneManager().FindScene(sceneName4), NULL);
	}

    /**
	 * GetActiveScene/SetActiveScene test
	 */
	TEST_F(SceneManagerTest, GetActiveScene)
	{
        const std::string sceneName1 = "TestScene1";
        const std::string sceneName2 = "TestScene2";
        const std::string sceneName3 = "NotCreated";

        ASSERT_EQ((int)mEngine->GetSceneManager().GetActiveScene(), NULL);

        mEngine->GetSceneManager().SetActiveScene(sceneName3);
        ASSERT_EQ((int)mEngine->GetSceneManager().GetActiveScene(), NULL);

        mEngine->GetSceneManager().CreateScene(sceneName1);
        mEngine->GetSceneManager().SetActiveScene(sceneName1);
        ASSERT_NE((int)mEngine->GetSceneManager().GetActiveScene(), NULL);
        ASSERT_EQ(mEngine->GetSceneManager().GetActiveScene(), mEngine->GetSceneManager().FindScene(sceneName1));
        mEngine->GetSceneManager().DeleteScene(sceneName1);
        ASSERT_EQ((int)mEngine->GetSceneManager().GetActiveScene(), NULL);

        mEngine->GetSceneManager().DeleteScene(sceneName2);
        ASSERT_EQ((int)mEngine->GetSceneManager().GetActiveScene(), NULL);

        mEngine->GetSceneManager().CreateScene(sceneName1);
        mEngine->GetSceneManager().CreateScene(sceneName2);
        mEngine->GetSceneManager().SetActiveScene(sceneName1);
        ASSERT_EQ(mEngine->GetSceneManager().GetActiveScene(), mEngine->GetSceneManager().FindScene(sceneName1));
        mEngine->GetSceneManager().SetActiveScene(sceneName2);
        ASSERT_EQ(mEngine->GetSceneManager().GetActiveScene(), mEngine->GetSceneManager().FindScene(sceneName2));
        mEngine->GetSceneManager().DeleteScene(sceneName1);
        mEngine->GetSceneManager().DeleteScene(sceneName2);
	}

    /**
	 * GetAllScenes test
	 */
	TEST_F(SceneManagerTest, GetAllScenes)
	{
		const std::string sceneName1 = "TestScene1";
        const std::string sceneName2 = "TestScene2";

        ASSERT_EQ(mEngine->GetSceneManager().GetAllScenes().size(), 0);
        mEngine->GetSceneManager().CreateScene(sceneName1);
        ASSERT_EQ(mEngine->GetSceneManager().GetAllScenes().size(), 1);
        ASSERT_EQ(mEngine->GetSceneManager().GetAllScenes().at(0)->GetSceneName(), sceneName1);
        mEngine->GetSceneManager().CreateScene(sceneName2);
        ASSERT_EQ(mEngine->GetSceneManager().GetAllScenes().size(), 2);
        ASSERT_EQ(mEngine->GetSceneManager().GetAllScenes().at(0)->GetSceneName(), sceneName1);
        ASSERT_EQ(mEngine->GetSceneManager().GetAllScenes().at(1)->GetSceneName(), sceneName2);

        mEngine->GetSceneManager().DeleteScene(sceneName1);
        mEngine->GetSceneManager().DeleteScene(sceneName2);
	}
}
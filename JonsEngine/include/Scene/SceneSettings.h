#pragma once

namespace JonsEngine
{
	struct SceneSettings
	{
		enum class CullingStrategy
		{
			STANDARD,
			AGGRESSIVE
		} mSceneCullingStrategy;


		SceneSettings();
	};

	inline SceneSettings::SceneSettings() :
		mSceneCullingStrategy(CullingStrategy::STANDARD)
	{
	}
}
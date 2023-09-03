#pragma once

#include "../Scene/SceneManager.h"
namespace IHCEngine::Core
{
	class SceneManagerLocator
	{
	public:
		static SceneManager* GetSceneManager()
		{
			return systemInstance;
		}

		static void Provide(SceneManager* system)
		{
			systemInstance = system;
		}

	private:
		inline static SceneManager* systemInstance{ nullptr };
	};
}
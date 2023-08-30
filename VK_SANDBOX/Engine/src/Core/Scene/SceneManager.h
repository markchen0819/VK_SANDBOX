#pragma once
#include "Scene.h"

namespace IHCEngine::Core
{
	class SceneManager
	{
	public:

		SceneManager() : activeScene(nullptr), nextSceneToLoad(-1) {};

		template <typename T> // template for custom scenes
		void AddScene()
		{
			assert((std::is_base_of_v<Scene, T>), "Can't add type {0} to Scene Manager", typeid(T).name());
			scenesMap[scenesMap.size()] = std::make_unique<T>();
		}

		void Update();
		void Shutdown();

		Scene* GetActiveScene() { return activeScene; };
		Scene* GetSceneAt(int pos);
		Scene* GetSceneByName(const std::string& name);

		void SetNextSceneToLoad(int sceneIndex);
		void SetNextSceneToLoad(const std::string& sceneName);
		bool ShouldLoadNextScene();
		void LoadNextScene();

		void DeferDestroyGameObjects();

	private:

		std::map<int, std::unique_ptr<Scene>> scenesMap;
		Scene* activeScene;
		int nextSceneToLoad;
	};
}


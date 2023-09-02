#pragma once
#include <map>
#include "Scene.h"

namespace IHCEngine::Core
{
	class SceneManager
	{
	public:

		SceneManager() : activeScene(nullptr), nextSceneToLoad(-1) {};
		~SceneManager();

		template <typename T> // template for custom scenes
		void AddScene()
		{
			if (!std::is_base_of_v<Scene, T>) 
			{
				std::cerr << "Can't add type " << typeid(T).name() << " to Scene Manager" << std::endl;
				assert(false);
			}
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


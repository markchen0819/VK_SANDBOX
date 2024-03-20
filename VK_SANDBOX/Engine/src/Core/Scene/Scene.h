#pragma once
#include <string>
#include <map>
// include the ones that will also be used in inherited scene
#include "GameObject.h"
#include "../../Graphics/Camera.h"

namespace IHCEngine::Core
{
	class Scene
	{
	public:
		Scene(std::string sceneName);
		virtual ~Scene() = default;

		// resources
		virtual void Load() = 0;
		virtual void UnLoad() = 0;

		// execution
		virtual void Init() = 0;
		void Update();
		void Shutdown();
		virtual void Reset() = 0;

		std::string GetName() { return sceneName; }
		//std::map<unsigned int, GameObject*> GetGameObjectsMap();
		std::map<unsigned int, std::unique_ptr<GameObject>>& GetGameObjectsMap() { return gameObjectsMap; }
		// GameObject helper functions
		GameObject& AddGameObject(const std::string& name);
		void RemoveGameObject(GameObject& gameObject);
		void RemoveAllGameObject();
		GameObject* GetGameObjectByName(const std::string& name);
		GameObject* GetGameObjectByUID(unsigned int id);
		// Scene graph
		std::vector<GameObject*>& GetRootGameObjects(); 
		void HierachyChanged() { hasHierachyChanged = true; };

		// Camera helper function
		IHCEngine::Graphics::Camera& GetCamera() { return mainCamera; };

	protected:

		std::string sceneName;

		// Camera
		IHCEngine::Graphics::Camera mainCamera
		{
			IHCEngine::Graphics::CameraType::PERSPECTIVE,
			glm::radians(45.0f),
			1280.0f/720.0f,//renderer->GetAspectRatio()
			0.1f,
			1000.0f,
			800,
			600
		};;

		// GameObjects
		// keep track of all gameobjects
		friend class GameObject;
		std::map<unsigned int, std::unique_ptr<GameObject>> gameObjectsMap;
		inline static unsigned int gobjIDCounter = 0;
		std::vector<GameObject*> rootGameObjects;
		bool hasHierachyChanged = false;

		friend class SceneManager;
		void DeferDestroyGameObjects();

	};
}


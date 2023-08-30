#pragma once
#include <string>
#include <map>
#include "GameObject.h"

//
//// Forward declaration
//namespace IHCEngine::Core
//{
//	class GameObject;
//}

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
		std::map<unsigned int, GameObject*> GetGameObjectsMap();

		// GameObject helper functions
		GameObject& AddGameObject(const std::string& name);
		void RemoveGameObject(GameObject& gameObject);
		GameObject* GetGameObjectByName(const std::string& name);
		GameObject* GetGameObjectByUID(unsigned int id);
		std::vector<GameObject*>& GetRootGameObjects(); //support scenegraph


	protected:

		std::string sceneName;

		// keep track of all gameobjects
		friend class GameObject;
		std::map<unsigned int, std::unique_ptr<GameObject>> gameObjectsMap;
		inline static unsigned int gobjIDCounter = 0;
		std::vector<GameObject*> rootGameObjects;
	
		friend class SceneManager;
		void DeferDestroyGameObjects();
	};
}


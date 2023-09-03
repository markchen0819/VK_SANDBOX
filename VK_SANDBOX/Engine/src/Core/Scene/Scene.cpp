#include "../../pch.h"
#include "Scene.h"
#include "GameObject.h"

// Temporary
#include "../Locator/Locators.h"
#include "../../Engine/src/Window/AppWindow.h"

IHCEngine::Core::Scene::Scene(std::string sceneName)
	: sceneName{ sceneName }
{ }

void IHCEngine::Core::Scene::Update()
{
	const auto& rootGameObjects = GetRootGameObjects();

	for (GameObject* parent : rootGameObjects)
	{
		if (parent->IsActive())
		{
			glm::mat4 parentLocalModelMatrix = parent->transform.GetLocalModelMatrix();
			for (int i = 0; i < parent->transform.GetChildCount(); ++i)
			{
				IHCEngine::Component::Transform* childTransform = parent->transform.GetChildAt(i);
				childTransform->PropagateParentLocalTransform(parentLocalModelMatrix);
			}
		}
	}

	// Temporary
	auto w = IHCEngine::Core::AppWindowLocator::GetAppWindow();
	mainCamera.cameraController.HandleInput(w->GetWindowHandle());
}

void IHCEngine::Core::Scene::Shutdown()
{
	// Destroy all game objects in the scene
	const auto& updatedRootGameObjects = GetRootGameObjects();
	for (GameObject* parent : updatedRootGameObjects)
	{
		parent->DestroyGameObject(); //destroy childs also with scene graph
	}
	rootGameObjects.clear();
}

std::map<unsigned int, IHCEngine::Core::GameObject*> IHCEngine::Core::Scene::GetGameObjectsMap()
{
	std::map<unsigned int, IHCEngine::Core::GameObject*> gameObjects;
	for (const auto& [key, value] : gameObjectsMap)
	{
		gameObjects[key] = value.get();
	}
	return gameObjects;
}

IHCEngine::Core::GameObject& IHCEngine::Core::Scene::AddGameObject(const std::string& name)
{
    auto gobj = std::make_unique<GameObject>(gobjIDCounter, name, this);
    auto iter = gameObjectsMap.insert({ gobjIDCounter, std::move(gobj) }); // iter first->element, second->bool if insertion took place
    ++gobjIDCounter;
	HierachyChanged();
    return *(iter.first->second);
}

void IHCEngine::Core::Scene::RemoveGameObject(GameObject& gameObject)
{
    unsigned int id = gameObject.GetUID();

    if (gameObjectsMap.find(id) == gameObjectsMap.end())
    {
        assert("Trying to remove game object with id {0} that is not in the scene", id);
    }
    gameObjectsMap.erase(id);
	HierachyChanged();
}

IHCEngine::Core::GameObject* IHCEngine::Core::Scene::GetGameObjectByName(const std::string& name)
{
	for (auto& pair : gameObjectsMap)
	{
		if (pair.second->GetName() == name)
		{
			return pair.second.get();
		}
	}
	std::cout << "GameObject with name: " << name << " not found" << std::endl;
	return nullptr;
}

IHCEngine::Core::GameObject* IHCEngine::Core::Scene::GetGameObjectByUID(unsigned int id)
{
	for (auto& pair : gameObjectsMap)
	{
		if (pair.second->GetUID() == id)
		{
			return pair.second.get();
		}
	}
	std::cout << "GameObject with UID: " << id << " not found" << std::endl;
	return nullptr;
}

std::vector<IHCEngine::Core::GameObject*>& IHCEngine::Core::Scene::GetRootGameObjects()
{
	if (hasHierachyChanged)
	{
		rootGameObjects.clear();
		for (auto& pair : gameObjectsMap)
		{
			if (pair.second->transform.GetParent() == nullptr)
			{
				rootGameObjects.push_back(pair.second.get());
			}
		}
		hasHierachyChanged == false;
	}
	return rootGameObjects;
}

void IHCEngine::Core::Scene::DeferDestroyGameObjects()
{
	const auto& updatedRootGameObjects = GetRootGameObjects();
	for (GameObject* parent : updatedRootGameObjects)
	{
		if (parent->shouldDestroy)
		{
			parent->DestroyGameObject(); //destroy childs also with scene graph
		}
	}
}

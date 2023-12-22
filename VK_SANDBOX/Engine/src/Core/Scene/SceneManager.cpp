#include "../../pch.h"
#include "SceneManager.h"
#include "Scene.h"

IHCEngine::Core::SceneManager::~SceneManager() = default;

void IHCEngine::Core::SceneManager::Update()
{
	// Scene Manager active scene is null on update
	assert(activeScene != nullptr);
	activeScene->Update();
}
void IHCEngine::Core::SceneManager::Shutdown()
{
	if (activeScene != nullptr)
	{
		activeScene->Shutdown(); // Kill all gameobjects
		activeScene->UnLoad(); // Release resource
	}
	activeScene = nullptr;
}

IHCEngine::Core::Scene* IHCEngine::Core::SceneManager::GetSceneAt(int pos)
{
	if (scenesMap.find(pos) == scenesMap.end())
	{
		//"Scene Manager does not contain scene at index {0}", id
		assert(true);
	}
	return scenesMap[pos].get();
}
IHCEngine::Core::Scene* IHCEngine::Core::SceneManager::GetSceneByName(const std::string& name)
{
	for (const auto& [id, scenePtr] : scenesMap)
	{
		if (scenePtr->GetName() == name)
		{
			return scenePtr.get();
		}
	}
	std::string tmp = "Scene Manager does not contain a scene with the name: " + name;
	assert(true);
	return nullptr; 
}
void IHCEngine::Core::SceneManager::SetNextSceneToLoad(int sceneIndex)
{
	if (scenesMap.find(sceneIndex) == scenesMap.end())
	{
		// "Scene Manager does not contain scene at index {0}", sceneIndex;
		assert(true);
	}
	nextSceneToLoad = sceneIndex;
}
void IHCEngine::Core::SceneManager::SetNextSceneToLoad(const std::string& sceneName)
{
	for (const auto& [id, scenePtr] : scenesMap)
	{
		if (scenePtr->GetName() == sceneName)
		{
			nextSceneToLoad = id;
			break;
		}
	}
	// "Scene Manager does not contain scene with name {0}", sceneName
	assert(true);
}
bool IHCEngine::Core::SceneManager::ShouldLoadNextScene()
{
	return nextSceneToLoad != -1;
}
void IHCEngine::Core::SceneManager::LoadNextScene()
{
	if (activeScene != nullptr)
	{
		activeScene->Shutdown(); // Kill all gameobjects
		activeScene->UnLoad(); // Release resources
	}

	activeScene = scenesMap.at(nextSceneToLoad).get();
	activeScene->Load(); // Load resources
	activeScene->Init(); // Create GameObjects
	activeScene->Update();

	nextSceneToLoad = -1;
}
void IHCEngine::Core::SceneManager::DeferDestroyGameObjects()
{
	activeScene->DeferDestroyGameObjects();
}

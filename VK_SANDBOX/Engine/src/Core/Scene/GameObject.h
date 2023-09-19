#pragma once
#include <vector>
#include "Components/Transform.h"
#include "Components/CustomBehavior/CustomBehavior.h"
#include "../../Graphics/Animation/Animation.h"
#include "../../Graphics/Animation/Animator.h"
// Forward declaration
namespace IHCEngine::Graphics 
{
	class Animator;
	class IHCMesh; 
    class IHCTexture;
    class Model;
}
namespace IHCEngine::Core
{
    class Scene;
}

namespace IHCEngine::Core
{
    class GameObject
    {
    public:

        GameObject(unsigned int id, std::string name, Scene* scene);
        ~GameObject() = default;
        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;

        unsigned int GetUID() { return uid; }
        std::string GetName() { return name; }
        bool IsActive() { return isActive; }
        void SetActive(bool active) { isActive = active; }
        void Destroy();// mark as destroy

        Scene* GetScene() { return scene; }

        IHCEngine::Component::Transform transform;

        // Component
		template<class T>
		T* AddComponent();
		template<class T>
		T* GetComponent();
		template <typename T>
        void RemoveComponent();
		template<class T>
        bool HasComponent();

    private:
        
        void DestroyGameObject(); // the actual destroy when defer destory

        unsigned int uid;
        std::string name = "defaultGobj";
        bool isActive = true;
        bool shouldDestroy = false;

        std::vector<std::unique_ptr<IHCEngine::Component::Component>> components;
        std::vector<IHCEngine::Component::CustomBehavior*> customBehaviors;
        std::vector<Component::Component*> componentList;

        friend class Scene;
        Scene* scene;
    };
}
#include "GameObject.inl"
#pragma once
#include "../../Transform/Transform.h"
#include "../../Graphics/VKWraps/IHCModel.h"
#include "../../Graphics/VKWraps/IHCTexture.h"
#include "Component.h"
#include "Scene.h"

// Forward declaration
namespace IHCEngine::Graphics 
{
    class IHCModel; 
    class IHCTexture; 
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

        IHCEngine::Transform::Transform transform;
        // Temporary Components
        IHCEngine::Graphics::IHCModel* model = nullptr;
        IHCEngine::Graphics::IHCTexture* texture = nullptr;

        // Component
		template<class T>
		void AddComponent();
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
        std::vector<std::unique_ptr<Component>> components;

        friend class Scene;
        Scene* scene;
    };
}
#include "GameObject.inl"
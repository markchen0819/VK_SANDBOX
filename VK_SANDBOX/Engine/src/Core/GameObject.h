#pragma once
#include "../Transform/Transform.h"
#include "../Graphics/IHCModel.h"
#include "../Graphics/IHCTexture.h"
#include "Component.h"

namespace IHCEngine::Core
{
    class GameObject
    {
    public:

        // Static Helper Functions
        static GameObject CreateGameObject()
        {
            static unsigned int currentId = 0;
            return GameObject{ currentId++ };
        }

        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;


        unsigned int GetUID() { return uid; }
        IHCEngine::Transform::Transform transform;


        // Component
		template<class T>
		void AddComponent();
		template<class T>
		T* GetComponent();
		template <typename T>
        void RemoveComponent();
		template<class T>
        bool HasComponent();

        // Temporary Components
        std::shared_ptr<IHCEngine::Graphics::IHCModel> model{};
        std::shared_ptr<IHCEngine::Graphics::IHCTexture> texture{};

    private:

        GameObject(unsigned int id) : uid{ id } {};
        std::vector<std::unique_ptr<Component>> components;
        unsigned int uid;


    };
}
#include "GameObject.inl"
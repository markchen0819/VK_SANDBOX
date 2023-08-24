#pragma once
#include "Component.h"
#include "../Transform/Transform.h"

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

        // std::unordered_map<unsigned int, GameObject>;

        GameObject(const GameObject&) = delete;
        GameObject& operator=(const GameObject&) = delete;

        unsigned int GetUID() { return uid; }
        IHCEngine::Transform::Transform transform;

		template<class T>
		void AddComponent();
		template<class T>
		T* GetComponent();
		template <typename T>
        void RemoveComponent();
		template<class T>
        bool HasComponent();

    private:

        GameObject(unsigned int id) : uid{ id } {};
        std::vector<std::unique_ptr<Component>> components;
        unsigned int uid;

        // Optional pointer components
        std::shared_ptr<IHCEngine::Graphics::IHCModel> model{};
    };
}
#include "GameObject.inl"
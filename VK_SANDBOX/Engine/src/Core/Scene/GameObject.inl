#pragma once
#include "../Locator/CustomBehaviorManagerLocator.h"
#include "Components/Component.h"

// Mosty using ECS approach
// 
// Engine Components:
// - Transform is auto added as local variable in GameObjects
// there for not using Add/ Remove Component
//
// Custom Components:
// Managed by CustomBehaviorManager
//
// all components are stored in 
// std::vector<std::unique_ptr<IHCEngine::Component::Component>> components;
// CustomBehaviorManager uses raw ptrs to use it
//

namespace IHCEngine::Core
{
	template<class T>
	T* GameObject::AddComponent()
	{
		if (HasComponent<T>())
		{
			std::cerr << "Error: GameObject already has a component of type: " << typeid(T).name() << std::endl;
			assert(false);
		}
		
		std::unique_ptr<Component::Component> component = std::make_unique<T>();
		component->SetOwner(this);
		component->Attach();

		// Custom Components we give raw ptrs for access
		Component::CustomBehavior* customBehavior = dynamic_cast<Component::CustomBehavior*>(component.get());
		if (customBehavior != nullptr)
		{
			CustomBehaviorManagerLocator::GetCustomBehaviorManager()
				->AddBehavior(customBehavior);
			customBehaviors.push_back(customBehavior);
		}
		T* rawptr = dynamic_cast<T*>(component.get());
		components.push_back(std::move(component));
		return rawptr;
	}

	template<class T>
	T* GameObject::GetComponent()
	{
		static_assert(std::is_base_of<Component::Component, T>::value, "T must derive from Component");
		for (auto& component : components)
		{
			if (dynamic_cast<T*>(component.get())) 
			{
				return static_cast<T*>(component.get());
			}
		}
		//std::cout << "Null Component: " << typeid(T).name() << std::endl;
		return nullptr;
	}

	template <typename T>
	void GameObject::RemoveComponent()
	{
		static_assert(typeid(T) != typeid(Component::Transform), "Removing Transform component from gameobject is not allowed");
		static_assert(std::is_base_of<Component::Component, T>::value, "T must derive from Component");

		auto it = std::find_if(components.begin(), components.end(),
			[](const std::unique_ptr<Component::Component>& component)
		{
			return dynamic_cast<T*>(component.get()) != nullptr;
		});


		if (it != components.end()) 
		{
			Component::Component* component = it;

			// Remove from Custom Component access
			Component::CustomBehavior* customBehavior = dynamic_cast<Component::CustomBehavior*>(component.get());
			if (customBehavior != nullptr)
			{
				CustomBehaviorManagerLocator::GetCustomBehaviorManager()
					->RemoveBehavior(customBehavior);
				customBehaviors.erase(customBehavior);
			}

			// Remove component from system (not including CustomBehaviorManager)
			it->Remove();
			components.erase(it);
		}
		else 
		{
			std::cout << "Component not found." << std::endl;
		}
	}

	template<class T>
	bool GameObject::HasComponent()
	{
		for (const auto& component : components)
		{
			if (dynamic_cast<T*>(component.get()))
			{
				return true;
			}
		}
		return false;
	}

}


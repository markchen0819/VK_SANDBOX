#pragma once
namespace IHCEngine::Core
{
	template<class T>
	void GameObject::AddComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
		if (HasComponent())
		{
			// typeid(T).name()
			static_assert("GameObject already has one component");
		}
		components.push_back(std::make_unique<T>(this));
	}

	template<class T>
	T* GameObject::GetComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
		for (auto& component : components)
		{
			if (dynamic_cast<T*>(component.get())) 
			{
				return static_cast<T*>(component.get());
			}
		}
		std::cout << "Null Component: " << typeid(T).name() << std::endl;
		return nullptr;
	}

	template <typename T>
	void GameObject::RemoveComponent()
	{
		static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");
		auto it = std::remove_if(components.begin(), components.end(),
			[](const std::unique_ptr<Component>& component)
		{
			return dynamic_cast<T*>(component.get()) != nullptr;
		});
		components.erase(it, components.end());
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
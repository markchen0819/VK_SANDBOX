#pragma once
#include "../pch.h"

namespace IHCEngine::Core
{
	class GameObject;  // Forward declaration

	enum class ComponentType 
	{
		Base,      
		Transform,  
		Renderer,   
		// add more component types as needed
	};

	class Component
	{
	public:
		Component(GameObject& owner,
			ComponentType type = ComponentType::Base)
			: owner(owner), isActive(true), type(type) {};
		virtual ~Component() = default;

		virtual void Init() {}
		virtual void Update() {}
		virtual void Destroy() {}

		GameObject& GetOwner() const { return owner; }
		ComponentType GetType() const { return type; }
		bool IsActive() const { return isActive; }
		void SetActive(bool activeStatus) { isActive = activeStatus; }
	
	private:
		ComponentType type;
		bool isActive;

		friend class GameObject;
		GameObject& owner;
	};

}


#pragma once
// Forward declaration
namespace IHCEngine::Core
{
	class GameObject;  
}

namespace IHCEngine::Component
{
	enum class ComponentType 
	{
		Base,      
		Transform,
		Pipeline,
		Mesh,
		Texture,
		Model,
		Animator,
		CustomBehavior,
		ImguiContext,
		// add more component types as needed
	};

	class Component
	{
	public:

		Component()
			: gameObject(nullptr), type(ComponentType::Base), isActive(true)
		{ };
		Component(ComponentType type)
			: gameObject(nullptr), type(type), isActive(true)
		{ }
		virtual ~Component() = default;


		IHCEngine::Core::GameObject& GetOwner() const { return *gameObject; }
		void SetOwner(IHCEngine::Core::GameObject* gobj) { gameObject = gobj; }
		ComponentType GetType() const { return type; }
		bool IsActive() const { return isActive; }
		void SetActive(bool activeStatus) { isActive = activeStatus; }
	
	protected:
		ComponentType type;
		bool isActive;

		IHCEngine::Core::GameObject* gameObject;

		friend class Core::GameObject;
		virtual void Remove() = 0; // Remove from locator if used
	};

}


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
		CustomBehavior,
		// add more component types as needed
	};

	class Component
	{
	public:

		Component(IHCEngine::Core::GameObject* owner)
			: gameObject(owner), isActive(true)
		{ };

		Component(ComponentType type)
			: gameObject(nullptr), type(type), isActive(true)
		{ }

		virtual ~Component() = default;

		virtual void Init() {}
		virtual void Update() {}
		virtual void Destroy() {}

		IHCEngine::Core::GameObject& GetOwner() const { return *gameObject; }
		void SetOwner(IHCEngine::Core::GameObject* gobj) { gameObject = gobj; }
		ComponentType GetType() const { return type; }
		bool IsActive() const { return isActive; }
		void SetActive(bool activeStatus) { isActive = activeStatus; }
	
	protected:
		ComponentType type;
		bool isActive;

		friend class GameObject;
		IHCEngine::Core::GameObject* gameObject;
	};

}


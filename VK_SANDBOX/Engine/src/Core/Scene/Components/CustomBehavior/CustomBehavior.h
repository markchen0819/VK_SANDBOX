#pragma once
#include "../Component.h"

namespace IHCEngine::Component
{
	enum class BehaviorState
	{
		Awake,
		Start,
		Update,
		OnEnable,
		OnDisable,
	};

	class CustomBehavior : public Component
	{
	public:
		CustomBehavior()
			: Component(ComponentType::CustomBehavior), state(BehaviorState::Awake)
		{ }

		virtual void Awake() { }
		virtual void Start() { }
		virtual void Update() { }
		virtual void FixedUpdate() { }
		virtual void OnEnable() { }
		virtual void OnDisable() { }

		BehaviorState GetState() { return state; }
		bool IsActive();

	private:

		BehaviorState state;
		bool hasAwaked = false;
		bool hasStarted = false;

		friend class CustomBehaviorManager;

		void Attach() override;
		void Remove() override; // remove from system (ECS)
	};
}


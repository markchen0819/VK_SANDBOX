#include "../../../../pch.h"
#include "CustomBehaviorManager.h"
#include "CustomBehavior.h"

void IHCEngine::Component::CustomBehaviorManager::Init()
{
	for (auto it = behaviors.begin(); it != behaviors.end();)
	{
		IHCEngine::Component::CustomBehavior* behavior = *it;

		if (!behavior->IsActive())
		{
			++it;
			continue;
		}
		behavior->Awake();
		behavior->hasAwaked = true;
		behavior->OnEnable();
		behavior->state = BehaviorState::Start;
	}
}

// Like Unity monobehavior life cycle
void IHCEngine::Component::CustomBehaviorManager::Update()
{
	for (auto it = behaviors.begin(); it != behaviors.end();)
	{
		IHCEngine::Component::CustomBehavior* behavior = *it;

		auto state = behavior->GetState();
		if (state == BehaviorState::Update)
		{
			if (behavior->IsActive()) // both gobj and component active
			{
				behavior->Update();
			}
			else if (!behavior->isActive) // component not active
			{
				behavior->state = BehaviorState::OnDisable;
			}
		}
		else if (state == BehaviorState::Awake)
		{
			behavior->Awake();
			behavior->hasAwaked = true;
			behavior->OnEnable();
			behavior->state = BehaviorState::Start;
		}
		else if (state == BehaviorState::Start)
		{
			behavior->Start();
			behavior->hasStarted = true;
			behavior->state = BehaviorState::Update;
		}
		else if (state == BehaviorState::OnEnable)
		{
			behavior->OnEnable();
			if (behavior->hasAwaked) 
			{
				if (behavior->hasStarted)
				{
					behavior->state = BehaviorState::Update;
					continue;
				}
				behavior->state = BehaviorState::Start;
				continue;
			}
			behavior->state = BehaviorState::Awake;
			continue;
		}
		else if (state == BehaviorState::OnDisable)
		{
			behavior->OnDisable();
			if (behavior->IsActive()) // both gobj and component active
			{
				behavior->state = BehaviorState::OnEnable;
			}
		}
	}
}

void IHCEngine::Component::CustomBehaviorManager::FixedUpdate()
{
	for (auto it = behaviors.begin(); it != behaviors.end();)
	{
		IHCEngine::Component::CustomBehavior* behavior = *it;
		auto state = behavior->GetState();
		if (state == BehaviorState::Update)
		{
			behavior->FixedUpdate();
		}
	}
}

void IHCEngine::Component::CustomBehaviorManager::AddBehavior(CustomBehavior* behavior)
{
	behaviors.insert(behavior);
	behavior->state = BehaviorState::Awake;
}

void IHCEngine::Component::CustomBehaviorManager::RemoveBehavior(CustomBehavior* behavior)
{
	behaviors.erase(behavior);
}

void IHCEngine::Component::CustomBehaviorManager::Reset()
{
	behaviors.clear();
}

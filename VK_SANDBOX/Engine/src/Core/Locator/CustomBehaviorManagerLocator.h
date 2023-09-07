#pragma once

#include "../Scene/Components/CustomBehavior/CustomBehaviorManager.h"
namespace IHCEngine::Core
{
	class CustomBehaviorManagerLocator
	{
	public:
		static Component::CustomBehaviorManager* GetCustomBehaviorManager()
		{
			return systemInstance;
		}

		static void Provide(Component::CustomBehaviorManager* system)
		{
			systemInstance = system;
		}

	private:
		inline static Component::CustomBehaviorManager* systemInstance{ nullptr };
	};
}
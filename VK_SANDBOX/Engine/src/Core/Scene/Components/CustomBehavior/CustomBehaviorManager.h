#pragma once
namespace IHCEngine::Component
{
	class CustomBehavior;

	class CustomBehaviorManager
	{
	public:
		CustomBehaviorManager() = default;
		~CustomBehaviorManager() = default;

		void Init();
		void Update();
		void FixedUpdate();

		void AddBehavior(CustomBehavior* behavior);
		void RemoveBehavior(CustomBehavior* behavior);

		void Reset();

	private:

		std::unordered_set<CustomBehavior*> behaviors;
	};
}

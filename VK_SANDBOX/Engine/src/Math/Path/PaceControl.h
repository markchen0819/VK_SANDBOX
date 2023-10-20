#pragma once
#include "../../Core/Locator/AssetManagerLocator.h"

namespace IHCEngine::Math
{
	// Sliding and skidding control
	// Synchronize animation speed with movement speed

	struct PaceControl
	{
		float GetAnimatorSpeedModifier(std::string animationID, float currentSpeed, float totalDistance, float totalTime)
		{
			auto animation = Core::AssetManagerLocator::GetAssetManager()->GetAnimationRepository().GetAsset(animationID);

			float cycleDuration = animation->GetTicksPerSecond() / animation->GetDuration();
			float normalSpeed = totalDistance/ totalTime;
			float idealPace = normalSpeed * cycleDuration;
			float currentPace = currentSpeed * cycleDuration;
			float animationSpeedModifier = currentPace / idealPace;
			return animationSpeedModifier;
		}
	};

	// Cycle: complete sequence of an animation, walk foot right then left
	// Ticks: time animation timeline
	// TicksPerSecond: map ticks to actual elapsed time
	// Duration: cycle (in animation timeline)

	// Divide TicksPerSecond Animation'sDuration -> one cycle in real life time
}

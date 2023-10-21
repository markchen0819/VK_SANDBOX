#pragma once

namespace IHCEngine::Graphics
{
	class Animation;

	class BlendTree
	{
	public:

		void SetAnimationA(Animation* animation) { animationA = animation; };
		void SetAnimationB(Animation* animation) { animationB = animation; };
		Animation* GetAnimationA() { return animationA; };
		Animation* GetAnimationB() { return animationB; };

		void SetUpperBoundSpeed(float s) { upperBoundSpeed = s; }
		void SetLowerBoundSpeed(float s) { lowerBoundSpeed = s; }

		// Call below in order
		void SetCurrentSpeed(float s) { currentSpeed = s; }
		float GetBlendFactor();

	private:

		Animation* animationA = nullptr;
		Animation* animationB = nullptr;

		float currentSpeed = 0.0;
		float upperBoundSpeed = 0.0;
		float lowerBoundSpeed = 0.0;
		float blendFactor = 0.0;
	};
}

#pragma once
namespace IHCEngine::Graphics
{
	struct AssimpNodeData;
	class Animation;

	class Animator
	{
	public:
		Animator();
		~Animator() = default;

		void UpdateAnimation();
		void PlayAnimation(Animation* animation);

	private:

		void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
		std::vector<glm::mat4> GetFinalBoneMatrices();

		std::vector<glm::mat4> finalBoneMatrices;
		Animation* currentAnimation;
		float currentTime;
	};

}
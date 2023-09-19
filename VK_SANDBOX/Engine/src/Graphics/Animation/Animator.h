#pragma once
namespace IHCEngine::Graphics
{
	struct AssimpNodeData;
	class Animation;

	class Animator
	{
	public:
		Animator();
		~Animator();

		void UpdateAnimation(float dt);
		void PlayAnimation(Animation* animation);
		Animation* GetCurrentAnimation() { return currentAnimation; }
		std::vector<glm::mat4> GetFinalBoneMatrices();

	private:

		void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);


		std::vector<glm::mat4> finalBoneMatrices;
		Animation* currentAnimation;
		float currentTime;
	};

}
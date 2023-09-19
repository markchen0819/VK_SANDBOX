#pragma once
namespace IHCEngine::Graphics
{
	struct AssimpNodeData;
	class IHCBuffer;
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

		// Vulkan
		void SetDescriptorSets(std::vector<VkDescriptorSet> set) { descriptorSets = set; }
		std::vector<VkDescriptorSet>& GetDescriptorSets() { return descriptorSets; }
		void SetBuffers(std::vector<IHCBuffer*> buffers) { skeletalBuffers = buffers; }
		std::vector<IHCBuffer*>& GetBuffers() { return skeletalBuffers; }


	private:

		void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

		std::vector<glm::mat4> finalBoneMatrices;
		Animation* currentAnimation;
		float currentTime;

		// Vulkan
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<IHCBuffer*> skeletalBuffers;
	};

}
#pragma once

#include "../VKWraps/VKHelpers.h"

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

		std::vector<Vertex>& GetDebugBoneVertices() { return debugBoneVertices; }
	private:

		void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

		std::vector<glm::mat4> finalBoneMatrices;
		Animation* currentAnimation;
		float currentTime;

		// Vulkan
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<IHCBuffer*> skeletalBuffers;
		std::vector<IHCBuffer*> debugBoneBuffers;

		// Debug
		std::vector<Vertex> debugBoneVertices;

	};

}
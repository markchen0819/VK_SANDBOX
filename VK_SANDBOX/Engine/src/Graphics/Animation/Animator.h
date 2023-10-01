#pragma once

#include "../VKWraps/VKHelpers.h"
#include "../../Math/VQS.h"

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
		void SetAnimation(Animation* animation);
		void PlayAnimation();

		Animation* GetCurrentAnimation() { return currentAnimation; }
		std::vector<glm::mat4>& GetFinalBoneMatrices();

		void SetSpeed(float animatorSpeed);
		float GetSpeed() { return speed; }

		// Vulkan
		void SetDescriptorSets(std::vector<VkDescriptorSet> set) { descriptorSets = set; }
		std::vector<VkDescriptorSet>& GetDescriptorSets() { return descriptorSets; }
		void SetBuffers(std::vector<IHCBuffer*> buffers) { skeletalBuffers = buffers; }
		std::vector<IHCBuffer*>& GetBuffers() { return skeletalBuffers; }

		// Debug
		void AllocateDebugBoneBuffer();
		void UpdateDebugBoneBuffer(FrameInfo& frameInfo);
		void DrawDebugBoneBuffer(FrameInfo& frameInfo);

	private:

		void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
		void calculateBoneTransformVQS(const AssimpNodeData* node, Math::VQS parentTransform);

		std::vector<glm::mat4> finalBoneMatrices;
		Animation* currentAnimation = nullptr;
		float currentTime;
		float speed = 1.0f;

		// Vulkan
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<IHCBuffer*> skeletalBuffers;

		// Debug
		std::vector<Vertex> debugBoneVertices;
		std::vector<std::unique_ptr<IHCBuffer>> debugBoneBuffers;
	};

}

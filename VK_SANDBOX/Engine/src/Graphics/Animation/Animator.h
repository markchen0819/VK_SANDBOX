#pragma once

#include "../VKWraps/VKHelpers.h"
#include "../../Math/VQS.h"

namespace IHCEngine::Graphics
{
	class BlendTree;
	struct AssimpNodeData;
	class IHCBuffer;
	class Animation;

	enum class AnimationType
	{
		SINGLE_ANIMATION,
		BLEND_TREE,
	};

	class Animator
	{
	public:
		Animator();
		~Animator();

		void SetAnimationType(AnimationType type) { animationType = type; }

		void UpdateAnimation(float dt);
		void PlayAnimation();
		void StopAnimation();
		void SetSpeed(float animatorSpeed);
		float GetSpeed() { return speed; }

		// Animation
		void SetAnimation(Animation* animation);
		Animation* GetCurrentAnimation() { return currentAnimation; }

		// BlendTree
		void SetBlendTree(BlendTree* blendtree);
		BlendTree* GetCurrentBlendTree() { return blendTree; }

		// Drawing
		std::vector<glm::mat4>& GetFinalBoneMatrices();

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

		AnimationType animationType = AnimationType::SINGLE_ANIMATION;
		std::vector<glm::mat4> finalBoneMatrices;
		bool isPlaying = false;
		float speed = 1.0f;
		float currentTime;

		// Single Animation
		void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
		void calculateBoneTransformVQS(const AssimpNodeData* node, Math::VQS parentTransform);
		Animation* currentAnimation = nullptr;

		// BlendTree
		BlendTree* blendTree;
		void calculateBoneTransformVQS(BlendTree* blendTree, const AssimpNodeData* node, Math::VQS parentTransform);


		// Vulkan
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<IHCBuffer*> skeletalBuffers;
		// Debug
		std::vector<Vertex> debugBoneVertices;
		std::vector<std::unique_ptr<IHCBuffer>> debugBoneBuffers;
	};

}

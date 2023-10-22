#pragma once

#include "../VKWraps/VKHelpers.h"
#include "../../Math/VQS.h"

namespace IHCEngine::Graphics
{
	class BlendTree;
	struct SkeletalNodeData;
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

		void PlayAnimation();
		void UpdateAnimation(float dt);
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
		bool isPlaying = false;
		float currentTime;
		float speed = 1.0f;
		std::vector<glm::mat4> finalBoneMatrices;

		// Single Animation
		Animation* currentAnimation = nullptr;
		void calculateBoneTransformMatrix(const SkeletalNodeData* node, glm::mat4 parentTransform);
		void calculateBoneTransformVQS(const SkeletalNodeData* node, Math::VQS parentTransform);

		// BlendTree
		BlendTree* blendTree = nullptr;
		void calculateBoneTransformMatrix(BlendTree* blendTree, const SkeletalNodeData* node, glm::mat4 parentTransform);
		void calculateBoneTransformVQS(BlendTree* blendTree, const SkeletalNodeData* node, Math::VQS parentTransform);


		// Vulkan
		std::vector<VkDescriptorSet> descriptorSets;
		std::vector<IHCBuffer*> skeletalBuffers;
		// Debug
		std::vector<Vertex> debugBoneVertices;
		std::vector<std::unique_ptr<IHCBuffer>> debugBoneBuffers;
	};

}

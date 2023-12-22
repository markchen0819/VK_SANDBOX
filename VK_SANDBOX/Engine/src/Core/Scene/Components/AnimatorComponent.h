#pragma once
#include "Component.h"
#include "../../../Graphics/Animation/Animator.h"
#include "../../../Graphics/Animation/BlendTree.h"

namespace IHCEngine::Component
{
	class AnimatorComponent : public Component
	{
	public:
		AnimatorComponent();
		~AnimatorComponent() = default;

		void SetAnimationType(Graphics::AnimationType type);

		// Animation
		void SetAnimation(Graphics::Animation* animation);
		// BlendTree
		void SetBlendTree(Graphics::BlendTree* blendtree);

		void PlayAnimation();
		void StopAnimation();
		bool HasAnimation();

		void UpdateAnimation(float dt);
		std::vector<glm::mat4>& GetFinalBoneMatrices();

		float GetSpeed();
		void SetSpeed(float speed);

		std::vector<VkDescriptorSet>& GetDescriptorSets();
		std::vector<Graphics::IHCBuffer*>& GetBuffers();


		// IK
		void OverwriteAnimationLocalVQStoModelLocalVQS();

		// Debug
		std::vector<Vertex>& GetDebugBoneVertices() { return animator.GetDebugBoneVertices(); }

	private:

		IHCEngine::Graphics::Animator animator;

		void Attach() override;
		void Remove() override;
	};
}

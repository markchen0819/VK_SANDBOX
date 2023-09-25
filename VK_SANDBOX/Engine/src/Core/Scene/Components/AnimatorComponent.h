#pragma once
#include "Component.h"
#include "../../../Graphics/Animation/Animator.h"


namespace IHCEngine::Component
{
	class AnimatorComponent : public Component
	{
	public:
		AnimatorComponent();
		~AnimatorComponent() = default;

		void SetAnimation(Graphics::Animation* animation);
		void PlayAnimation();
		bool HasAnimation();

		void UpdateAnimation(float dt);
		std::vector<glm::mat4>& GetFinalBoneMatrices();

		std::vector<VkDescriptorSet>& GetDescriptorSets();
		std::vector<Graphics::IHCBuffer*>& GetBuffers();

		// Debug
		void AllocateDebugBoneBuffer();
		void UpdateDebugBoneBuffer(Graphics::FrameInfo& frameInfo);
		void DrawDebugBoneBuffer(Graphics::FrameInfo& frameInfo);


	private:

		IHCEngine::Graphics::Animator animator;

		void Remove() override;
	};
}

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

		void PlayAnimation(Graphics::Animation* animation);
		bool HasAnimation();

		void UpdateAnimation(float dt);
		std::vector<glm::mat4> GetFinalBoneMatrices();

		std::vector<VkDescriptorSet>& GetDescriptorSets();
		std::vector<Graphics::IHCBuffer*>& GetBuffers();

	private:

		IHCEngine::Graphics::Animator animator;

		void Remove() override;
	};
}

class AnimatorComponent
{

};


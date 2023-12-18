#include "../../../pch.h"
#include "AnimatorComponent.h"

#include "../../Locator/GraphicsManagerLocator.h"
#include "../../../Graphics/RenderSystems/RenderSystem.h"
#include "../../Engine/src/Graphics/VKWraps/IHCBuffer.h"
#include "../../Engine/src/Graphics/Animation/Animation.h"


IHCEngine::Component::AnimatorComponent::AnimatorComponent()
	:Component(ComponentType::Animator)
{

}

void IHCEngine::Component::AnimatorComponent::SetAnimationType(Graphics::AnimationType type)
{
	animator.SetAnimationType(type);
}

void IHCEngine::Component::AnimatorComponent::SetAnimation(Graphics::Animation* animation)
{
	animator.SetAnimation(animation);
}

void IHCEngine::Component::AnimatorComponent::SetBlendTree(Graphics::BlendTree* blendtree)
{
	animator.SetBlendTree(blendtree);
}

void IHCEngine::Component::AnimatorComponent::PlayAnimation()
{
	animator.PlayAnimation();
}

void IHCEngine::Component::AnimatorComponent::StopAnimation()
{
	animator.StopAnimation();
}

bool IHCEngine::Component::AnimatorComponent::HasAnimation()
{
	if(animator.GetCurrentAnimation()!=nullptr || animator.GetCurrentBlendTree() != nullptr)
	{
		return true;
	}
	return false;
}

void IHCEngine::Component::AnimatorComponent::SetSpeed(float speed)
{
	animator.SetSpeed(speed);
}

void IHCEngine::Component::AnimatorComponent::UpdateAnimation(float dt)
{
	animator.UpdateAnimation(dt);
}

std::vector<glm::mat4>& IHCEngine::Component::AnimatorComponent::GetFinalBoneMatrices()
{
	return animator.GetFinalBoneMatrices();
}

float IHCEngine::Component::AnimatorComponent::GetSpeed()
{
	return animator.GetSpeed();
}

std::vector<VkDescriptorSet>& IHCEngine::Component::AnimatorComponent::GetDescriptorSets()
{
	return animator.GetDescriptorSets();
}

std::vector<IHCEngine::Graphics::IHCBuffer*>& IHCEngine::Component::AnimatorComponent::GetBuffers()
{
	return animator.GetBuffers();
}

void IHCEngine::Component::AnimatorComponent::OverwriteAnimationLocalVQStoModelLocalVQS()
{
	auto& root = animator.GetCurrentAnimation()->GetRootNodeOfHierarhcy();
	animator.OverwriteAnimationLocalVQStoModelLocalVQS(&root);
}

void IHCEngine::Component::AnimatorComponent::Attach()
{
	Core::GraphicsManagerLocator::GetGraphicsManager()->GetRenderSystem().AddGameObjectToRender(this->gameObject, Graphics::PipelineType::SKELETAL);
}

void IHCEngine::Component::AnimatorComponent::Remove()
{
	Core::GraphicsManagerLocator::GetGraphicsManager()->GetRenderSystem().RemoveGameObjectToRender(this->gameObject, Graphics::PipelineType::SKELETAL);
}

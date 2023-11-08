#include "../../../pch.h"
#include "IKComponent.h"

#include "../../Locator/GraphicsManagerLocator.h"
#include "../../Engine/src/Graphics/VKWraps/IHCBuffer.h"
#include "../../Engine/src/Graphics/VKWraps/IHCDevice.h"

IHCEngine::Component::IKComponent::IKComponent()
	:Component(ComponentType::InverseKinematics)
{

}

void IHCEngine::Component::IKComponent::Update()
{
	IKSolver.Update();
}

void IHCEngine::Component::IKComponent::SetModel(Graphics::Model* m)
{
	IKSolver.SetModel(m);
}

void IHCEngine::Component::IKComponent::SetGameObjectVQS(Math::VQS vqs)
{
	IKSolver.SetGameObjectVQS(vqs);
}

void IHCEngine::Component::IKComponent::SetRootAndEE(std::string root, std::string EE)
{
	IKSolver.SetRootAndEE(root, EE);
}

void IHCEngine::Component::IKComponent::SetTarget(glm::vec3 target)
{
	IKSolver.SetTarget(target);
}

std::vector<glm::mat4>& IHCEngine::Component::IKComponent::GetFinalBoneMatrices()
{
	return IKSolver.GetFinalBoneMatrices();
}

std::vector<VkDescriptorSet>& IHCEngine::Component::IKComponent::GetDescriptorSets()
{
	return IKSolver.GetDescriptorSets();
}

std::vector<IHCEngine::Graphics::IHCBuffer*>& IHCEngine::Component::IKComponent::GetBuffers()
{
	return IKSolver.GetBuffers();
}

void IHCEngine::Component::IKComponent::AllocateDebugBoneBuffer()
{
	IKSolver.AllocateDebugBoneBuffer();
}

void IHCEngine::Component::IKComponent::UpdateDebugBoneBuffer(Graphics::FrameInfo& frameInfo)
{
	IKSolver.UpdateDebugBoneBuffer(frameInfo);
}

void IHCEngine::Component::IKComponent::DrawDebugBoneBuffer(Graphics::FrameInfo& frameInfo)
{
	IKSolver.DrawDebugBoneBuffer(frameInfo);
}

void IHCEngine::Component::IKComponent::Remove()
{

}

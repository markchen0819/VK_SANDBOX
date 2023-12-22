#include "../../../pch.h"
#include "IKComponent.h"

#include "../../Locator/GraphicsManagerLocator.h"
#include "../../../Graphics/RenderSystems/RenderSystem.h"
#include "../../Engine/src/Graphics/VKWraps/IHCBuffer.h"

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

float IHCEngine::Component::IKComponent::GetTotalDistance() const
{
	return IKSolver.GetTotalDistance();
}

const IHCEngine::Graphics::SkeletalNodeData* IHCEngine::Component::IKComponent::GetRoot() const
{
	return IKSolver.GetRoot();
}

const IHCEngine::Graphics::SkeletalNodeData* IHCEngine::Component::IKComponent::GetEndEffector() const
{
	return IKSolver.GetEndEffector();
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

std::vector<Vertex>& IHCEngine::Component::IKComponent::GetDebugBoneVertices()
{
	return IKSolver.GetDebugBoneVertices();
}

void IHCEngine::Component::IKComponent::Attach()
{
	Core::GraphicsManagerLocator::GetGraphicsManager()->GetRenderSystem().AddGameObjectToRender(this->gameObject, Graphics::PipelineType::SKELETAL);
}

void IHCEngine::Component::IKComponent::Remove()
{
	Core::GraphicsManagerLocator::GetGraphicsManager()->GetRenderSystem().RemoveGameObjectToRender(this->gameObject, Graphics::PipelineType::SKELETAL);
}

#include "../../../pch.h"
#include "ModelComponent.h"

#include "../../../Graphics/Animation/Model.h"
#include "../../../Graphics/VKWraps/IHCMesh.h"

IHCEngine::Component::ModelComponent::ModelComponent()
	:Component(ComponentType::Model)
{
	std::cout << std::endl;
}

void IHCEngine::Component::ModelComponent::SetModel(Graphics::Model* model)
{
	this->model = model;
}

std::unordered_map<std::string, IHCEngine::Graphics::IHCMesh*> IHCEngine::Component::ModelComponent::GetMeshes()
{
	return model->GetMeshes();
}

IHCEngine::Graphics::MaterialData IHCEngine::Component::ModelComponent::GetMaterialForMesh(std::string key)
{
	return model->GetMaterialForMesh(key);
}

void IHCEngine::Component::ModelComponent::Remove()
{

}
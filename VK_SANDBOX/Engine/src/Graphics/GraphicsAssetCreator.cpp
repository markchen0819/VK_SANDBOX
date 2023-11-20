#include "../pch.h"
#include "GraphicsAssetCreator.h"
#include "VKWraps/IHCTexture.h"
#include "VKWraps/IHCDescriptorManager.h"

#include "../Core/Locator/AssetManagerLocator.h"

namespace IHCEngine::Graphics
{
	GraphicsAssetCreator::GraphicsAssetCreator(IHCDevice& ihcDevice, IHCDescriptorManager* descriptorManager)
		: ihcDevice(ihcDevice),
          descriptorManager(descriptorManager)
	{
        
	}

    // Texture
    IHCTexture* GraphicsAssetCreator::CreateTexture(std::string assetName, std::string path)
    {
        // Create texture
        // Allocate descriptorSet
        // Move texture asset to AssetManager
        auto texture = std::make_unique<IHCTexture>(ihcDevice, assetName, path);
        auto rawPtr = texture.get();
		descriptorManager->AllocateTextureDescriptorSetForTexture(texture.get());
        auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
        assetManager->GetTextureRepository().AddAsset(assetName, std::move(texture));
        return rawPtr;
    }
    void GraphicsAssetCreator::DestroyTexture(std::string assetName)
    {
        // Create texture
        // Deallocate descriptorSet
        // Destroy texture asset in AssetManager
        auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
        auto ihcTexture = assetManager->GetTextureRepository().GetAsset(assetName);
        descriptorManager->DeallocateTextureDescriptorSetForTexture(ihcTexture);
        assetManager->GetTextureRepository().RemoveAsset(assetName);
    }

    // Mesh
    IHCMesh* GraphicsAssetCreator::CreateMesh(std::string assetName, std::string path)
    {
        auto mesh = IHCMesh::CreateMeshFromFile(ihcDevice, path);
        auto rawPtr = mesh.get();
        // No descriptor for meshes
		auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
		assetManager->GetMeshRepository().AddAsset(assetName,std::move(mesh));
        return rawPtr;
    }
    IHCMesh* GraphicsAssetCreator::CreateMesh(std::string assetName, IHCEngine::Graphics::IHCMesh::Builder& builder)
    {
        auto mesh = std::make_unique<IHCEngine::Graphics::IHCMesh>(ihcDevice, builder);
        auto rawPtr = mesh.get();
        // No descriptor for meshes
        auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
        assetManager->GetMeshRepository().AddAsset(assetName, std::move(mesh));
        return rawPtr;
    }
    void GraphicsAssetCreator::DestroyMesh(std::string assetName)
    {
        // No descriptor for meshes
        auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
        assetManager->GetMeshRepository().RemoveAsset(assetName);
    }

    // Model 
    Model* GraphicsAssetCreator::CreateModel(std::string assetName, std::string path)
    {
        auto model = std::make_unique<IHCEngine::Graphics::Model>(path);
        auto rawPtr = model.get();
        // No descriptor for model
        auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
        assetManager->GetModelRepository().AddAsset(assetName, std::move(model));
        return rawPtr;
    }

    void GraphicsAssetCreator::DestroyModel(std::string assetName)
    {
        // No descriptor for model
        auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
        assetManager->GetModelRepository().RemoveAsset(assetName);
    }

    Animation* GraphicsAssetCreator::CreateAnimation(std::string assetName, std::string path, Model* model)
    {
        auto animation = std::make_unique<IHCEngine::Graphics::Animation>(
            path, model);
        auto rawPtr = animation.get();
        // No descriptor for animation
        auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
        assetManager->GetAnimationRepository().AddAsset(assetName, std::move(animation));
        return rawPtr;
    }

    void GraphicsAssetCreator::DestroyAnimation(std::string assetName)
    {
        // No descriptor for animation
        auto assetManager = IHCEngine::Core::AssetManagerLocator::GetAssetManager();
        assetManager->GetAnimationRepository().RemoveAsset(assetName);
    }

    // Skeletal Data for Animator (vulkan ubo)
    void GraphicsAssetCreator::CreateSkeletalData(Animator* animator)
    {
        descriptorManager->AllocateSkeletalDescriptorSetForAnimator(animator);
    }
    void GraphicsAssetCreator::DestroySkeletalData(Animator* animator)
    {
        descriptorManager->DeallocateSkeletalDescriptorSetForAnimator(animator);
    }

    //  Skeletal Data for IK (vulkan ubo)
    void GraphicsAssetCreator::CreateSkeletalData(InverseKinematicsSolver* ikSolver)
    {
        descriptorManager->AllocateSkeletalDescriptorSetForIK(ikSolver);
    }
    void GraphicsAssetCreator::DestroySkeletalData(InverseKinematicsSolver* ikSolver)
    {
        descriptorManager->DeallocateSkeletalDescriptorSetForIK(ikSolver);
    }
}

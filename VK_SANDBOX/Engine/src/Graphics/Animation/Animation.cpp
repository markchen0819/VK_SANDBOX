#include "../../pch.h"
#include "Animation.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "AssimpGLMHelpers.h"
#include "Bone.h"
#include "Model.h"

IHCEngine::Graphics::Animation::Animation(const std::string animationPath, Model* model)
	: animationPath (animationPath),
	  model(model),
      boneInfoMap(nullptr)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    assert(scene && scene->mRootNode);
    auto animation = scene->mAnimations[0];
    duration = animation->mDuration;
    ticksPerSecond = animation->mTicksPerSecond;
    ReadHierarchyData(rootNode, scene->mRootNode);
    ReadMissingBones(animation, *model);

    std::cout << "============" << std::endl;
    std::cout << "Loading Animation:" << animationPath << std::endl;
    std::cout << "duration:" << duration << std::endl;
    std::cout << "ticksPerSecond:" << ticksPerSecond << std::endl;
    std::cout << "boneInfoMap count:" << boneInfoMap->size() <<std::endl;
    std::cout << "============" << std::endl;
}

IHCEngine::Graphics::Bone* IHCEngine::Graphics::Animation::FindBone(const std::string& name)
{
    auto iter = std::find_if(
        bones.begin(), 
        bones.end(),
        [&](const Bone& Bone)
    {
        return Bone.GetBoneName() == name;
    });
    if (iter == bones.end()) return nullptr;
    else return &(*iter);
}

void IHCEngine::Graphics::Animation::ReadMissingBones(const aiAnimation* animation, Model& model)
{
    int size = animation->mNumChannels;
    auto& boneInfoMap = model.GetBoneInfoMap();//getting boneInfoMap from Model class
    int& boneCount = model.GetBoneCount(); //getting the boneCounter from Model class
    // reading channels(bones engaged in an animation and their keyframes)
    for (int i = 0; i < size; i++)
    {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        // Update bones in Model
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            boneInfoMap[boneName].id = boneCount;
            ++boneCount;
        }
        // Create bones for the animation
        bones.push_back(Bone(channel->mNodeName.data,
            boneInfoMap[channel->mNodeName.data].id, channel));
    }
    // Saves the reference
    this->boneInfoMap = &boneInfoMap;
}

void IHCEngine::Graphics::Animation::ReadHierarchyData(AssimpNodeData& dest, const aiNode* src)
{
    assert(src);
    dest.name = src->mName.data;
    dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;
    for (int i = 0; i < src->mNumChildren; i++)
    {
        AssimpNodeData newData;
        ReadHierarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}

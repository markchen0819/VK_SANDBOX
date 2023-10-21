#include "../../pch.h"
#include "Animation.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "AssimpGLMHelpers.h"
#include "AnimationInfo.h"
#include "Model.h"
#include "BoneAnimation.h"

IHCEngine::Graphics::Animation::Animation(const std::string animationPath, Model* model)
	: animationPath (animationPath),
      model (model)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    assert(scene && scene->mRootNode);
    aiAnimation* animation = scene->mAnimations[0];
    duration = animation->mDuration;
    ticksPerSecond = animation->mTicksPerSecond;
    std::cout << "============" << std::endl;
    std::cout << "Loading Animation:" << animationPath << std::endl;
    std::cout << "duration:" << duration << std::endl;
    std::cout << "ticksPerSecond:" << ticksPerSecond << std::endl;
    std::cout << "============" << std::endl;
    std::cout << "Extract Animation for each bone:"<< std::endl;
    extractAnimationData(animation);
    std::cout << "Finished extraction" << std::endl;
}

void IHCEngine::Graphics::Animation::extractAnimationData(const aiAnimation* aiAnim)
{
    auto& skinningInfoMap = model->GetSkinningInfoMap();  // getting skinningInfoMap from Model class
    int& boneCount = model->GetBoneCount();               // getting the boneCounter from Model class

	for (int i = 0; i < aiAnim->mNumChannels; i++) // For each animation channels
    {
        aiNodeAnim* channel = aiAnim->mChannels[i]; 
        std::string boneName = channel->mNodeName.data;
        int boneID = skinningInfoMap[channel->mNodeName.data].id;

        // Extract Individual bone animation 
        std::vector<KeyPosition> keyPositions;
        std::vector<KeyRotation> keyRotations;
        std::vector<KeyScale> keyScales;
        const int numPositions = channel->mNumPositionKeys;
        const int numRotations = channel->mNumRotationKeys;
        const int numScales = channel->mNumScalingKeys;

        for (int positionIndex = 0; positionIndex < numPositions; ++positionIndex)
        {
            aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
            float timeStamp = channel->mPositionKeys[positionIndex].mTime;
            KeyPosition data;
            data.position = AssimpGLMHelpers::GetGLMVec3(aiPosition);
            data.timeStamp = timeStamp;
            keyPositions.push_back(data);
        }
        for (int rotationIndex = 0; rotationIndex < numRotations; ++rotationIndex)
        {
            aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
            float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
            KeyRotation data;
            data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
            data.timeStamp = timeStamp;
            keyRotations.push_back(data);
        }
        for (int keyIndex = 0; keyIndex < numScales; ++keyIndex)
        {
            aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
            float timeStamp = channel->mScalingKeys[keyIndex].mTime;
            KeyScale data;
            data.scale = AssimpGLMHelpers::GetGLMVec3(scale);
            data.timeStamp = timeStamp;
            keyScales.push_back(data);
        }

        // Update bones in Model
        if (skinningInfoMap.find(boneName) == skinningInfoMap.end())
        {
            // No match of bones between model & animation
            // create this bone for the model 
            skinningInfoMap[boneName].id = boneCount; // changed in model ref
            ++boneCount;                          // changed in model ref
        }
        boneAnimations.push_back(BoneAnimation
        (boneName, boneID, channel, keyPositions, keyRotations, keyScales));
    }
}

IHCEngine::Graphics::BoneAnimation* IHCEngine::Graphics::Animation::FindBone(const std::string& name)
{
    auto iter = std::find_if(
        boneAnimations.begin(),
        boneAnimations.end(),
        [&](const BoneAnimation& Bone)
    {
        return Bone.GetBoneName() == name;
    });
    if (iter == boneAnimations.end()) return nullptr;
    else return &(*iter);
}


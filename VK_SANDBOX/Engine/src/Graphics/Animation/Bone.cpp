#include "../../pch.h"
#include "Bone.h"
#include "AssimpGLMHelpers.h"


IHCEngine::Graphics::Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel)
    : name(name),
      id(ID),
	  localTransform(1.0f)
{
    //channel contains all bones and their keyframes
    // extract keyframe datas
    numPositions = channel->mNumPositionKeys; 
    for (int positionIndex = 0; positionIndex < numPositions; ++positionIndex)
    {
        aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
        float timeStamp = channel->mPositionKeys[positionIndex].mTime;
        KeyPosition data;
        data.position = AssimpGLMHelpers::GetGLMVec3(aiPosition);
        data.timeStamp = timeStamp;
        keyPositions.push_back(data);
    }
    numRotations = channel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < numRotations; ++rotationIndex)
    {
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
        KeyRotation data;
        data.orientation = AssimpGLMHelpers::GetGLMQuat(aiOrientation);
        data.timeStamp = timeStamp;
        keyRotations.push_back(data);
    }
    numScales = channel->mNumScalingKeys;
    for (int keyIndex = 0; keyIndex < numScales; ++keyIndex)
    {
        aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
        float timeStamp = channel->mScalingKeys[keyIndex].mTime;
        KeyScale data;
        data.scale = AssimpGLMHelpers::GetGLMVec3(scale);
        data.timeStamp = timeStamp;
        keyScales.push_back(data);
    }
}

void IHCEngine::Graphics::Bone::Update(float animationTime)
{
    glm::mat4 translation = interpolatePosition(animationTime);
    glm::mat4 rotation = interpolateRotation(animationTime);
    glm::mat4 scale = interpolateScaling(animationTime);
    localTransform = translation * rotation * scale;
}

int IHCEngine::Graphics::Bone::GetPositionIndex(float animationTime)
{
    for (int index = 0; index < numPositions - 1; ++index)
    {
        if (animationTime < keyPositions[index + 1].timeStamp)
            return index;
    }
    assert(0);
}
int IHCEngine::Graphics::Bone::GetRotationIndex(float animationTime)
{
    for (int index = 0; index < numRotations - 1; ++index)
    {
        if (animationTime < keyRotations[index + 1].timeStamp)
            return index;
    }
    assert(0);
}
int IHCEngine::Graphics::Bone::GetScaleIndex(float animationTime)
{
    for (int index = 0; index < numScales - 1; ++index)
    {
        if (animationTime < keyScales[index + 1].timeStamp)
            return index;
    }
    assert(0);
}


float IHCEngine::Graphics::Bone::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
    float scaleFactor = 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}
glm::mat4 IHCEngine::Graphics::Bone::interpolatePosition(float animationTime)
{
    if (numPositions==1)
    {
        return glm::translate(glm::mat4(1.0f), keyPositions[0].position);
    }

    int p0Index = GetPositionIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(
        keyPositions[p0Index].timeStamp,
        keyPositions[p1Index].timeStamp,
        animationTime);
    glm::vec3 finalPosition = 
        glm::mix(keyPositions[p0Index].position,
            keyPositions[p1Index].position,
            scaleFactor);
    return glm::translate(glm::mat4(1.0f), finalPosition);
}
glm::mat4 IHCEngine::Graphics::Bone::interpolateRotation(float animationTime)
{
    if (numRotations == 1)
    {
        auto rotation = glm::normalize(keyRotations[0].orientation);
        return glm::mat4_cast(rotation);
    }

    int p0Index = GetRotationIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(
        keyRotations[p0Index].timeStamp,
        keyRotations[p1Index].timeStamp,
        animationTime);
    glm::quat finalRotation = glm::slerp(
        keyRotations[p0Index].orientation,
        keyRotations[p1Index].orientation,
        scaleFactor);
    finalRotation = glm::normalize(finalRotation);
    return glm::mat4_cast(finalRotation);
}
glm::mat4 IHCEngine::Graphics::Bone::interpolateScaling(float animationTime)
{
    if (numScales == 1)
    {
        return glm::scale(glm::mat4(1.0f), keyScales[0].scale);
    }

    int p0Index = GetScaleIndex(animationTime);
    int p1Index = p0Index + 1;
    float scaleFactor = getScaleFactor(
        keyScales[p0Index].timeStamp,
        keyScales[p1Index].timeStamp,
        animationTime);
    glm::vec3 finalScale = glm::mix(
        keyScales[p0Index].scale,
        keyScales[p1Index].scale
        , scaleFactor);
    return glm::scale(glm::mat4(1.0f), finalScale);
}

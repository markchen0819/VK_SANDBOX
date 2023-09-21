#include "../../pch.h"
#include "BoneAnimation.h"
#include <assimp/anim.h>

namespace IHCEngine::Graphics
{
	BoneAnimation::BoneAnimation(const std::string& name, int boneID, const aiNodeAnim* channel,
		std::vector<KeyPosition> kP, std::vector<KeyRotation> kR, std::vector<KeyScale> kS)
		: boneName(name),
		  id(boneID),
		  channel(channel),
		  keyPositions(kP),
	      keyRotations(kR),
	      keyScales(kS),
		  localTransform(1.0f)
	{
		numPositions = channel->mNumPositionKeys;
		numRotations = channel->mNumRotationKeys;
		numScales = channel->mNumScalingKeys;
	}

	void BoneAnimation::Update(float animationTime)
	{
		glm::mat4 translation = interpolatePosition(animationTime);
		glm::mat4 rotation = interpolateRotation(animationTime);
		glm::mat4 scale = interpolateScaling(animationTime);
		localTransform = translation * rotation * scale;
	}

	int BoneAnimation::GetPositionIndex(float animationTime)
	{
		for (int index = 0; index < numPositions - 1; ++index)
		{
			if (animationTime < keyPositions[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}

	int BoneAnimation::GetRotationIndex(float animationTime)
	{
		for (int index = 0; index < numRotations - 1; ++index)
		{
			if (animationTime < keyRotations[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}

	int BoneAnimation::GetScaleIndex(float animationTime)
	{
		for (int index = 0; index < numScales - 1; ++index)
		{
			if (animationTime < keyScales[index + 1].timeStamp)
				return index;
		}
		assert(0);
	}

	float BoneAnimation::getScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime)
	{
		float scaleFactor = 0.0f;
		float midWayLength = animationTime - lastTimeStamp;
		float framesDiff = nextTimeStamp - lastTimeStamp;
		scaleFactor = midWayLength / framesDiff;
		return scaleFactor;
	}

	glm::mat4 BoneAnimation::interpolatePosition(float animationTime)
	{
		if (numPositions == 1)
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

	glm::mat4 BoneAnimation::interpolateRotation(float animationTime)
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

	glm::mat4 BoneAnimation::interpolateScaling(float animationTime)
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
}


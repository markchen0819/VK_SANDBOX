#include "../../pch.h"
#include "BoneAnimation.h"
#include <assimp/anim.h>
#include <glm/gtx/string_cast.hpp>

#include "AnimationConfig.h"

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
		if (AnimationConfig::calculateBonesWithVQS)
		{
			localVQS = interpolateVQS(animationTime);
		}
		else
		{
			glm::mat4 translation = interpolatePosition(animationTime);
			glm::mat4 rotation = interpolateRotation(animationTime);
			glm::mat4 scale = interpolateScaling(animationTime);
			localTransform = translation * rotation * scale;
		}
	}

#pragma region Calculate bones with GLM mat4
	int BoneAnimation::GetPositionIndex(float animationTime)
	{
		for (int index = 0; index < numPositions - 1; ++index)
		{
			if (animationTime < keyPositions[index + 1].timeStamp)
				return index;
		}
		assert(0);
		return -1;
	}
	int BoneAnimation::GetRotationIndex(float animationTime)
	{
		for (int index = 0; index < numRotations - 1; ++index)
		{
			if (animationTime < keyRotations[index + 1].timeStamp)
				return index;
		}
		assert(0);
		return -1;
	}
	int BoneAnimation::GetScaleIndex(float animationTime)
	{
		for (int index = 0; index < numScales - 1; ++index)
		{
			if (animationTime < keyScales[index + 1].timeStamp)
				return index;
		}
		assert(0);
		return -1;
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
#pragma endregion

#pragma region Calculate bones with VQS
	glm::vec3 BoneAnimation::GetVQSIndices(float animationTime)
	{
		// position
		int p0Index;
		int p1Index;
		if (numPositions == 1)
		{
			p0Index = p1Index = 0;
		}
		else
		{
			p0Index = GetPositionIndex(animationTime);
			p1Index = p0Index + 1;
		}
		float posFactor = getScaleFactor
		(
			keyPositions[p0Index].timeStamp,
			keyPositions[p1Index].timeStamp,
			animationTime
		);
		// rotation
		int r0Index;
		int r1Index;
		if (numRotations == 1)
		{
			r0Index = r1Index = 0;
		}
		else
		{
			r0Index = GetRotationIndex(animationTime);
			r1Index = r0Index + 1;
		}
		float rotFactor = getScaleFactor
		(
			keyRotations[r0Index].timeStamp,
			keyRotations[r1Index].timeStamp,
			animationTime
		);
		//scale
		int s0Index;
		int s1Index;
		if (numScales == 1)
		{
			s0Index = s1Index = 0;
		}
		else
		{
			s0Index = GetScaleIndex(animationTime);
			s1Index = s0Index + 1;
		}
		float scaleFactor = getScaleFactor
		(
			keyScales[s0Index].timeStamp,
			keyScales[s1Index].timeStamp,
			animationTime
		);

		return glm::vec3(posFactor, rotFactor, scaleFactor);
	}
	Math::VQS BoneAnimation::interpolateVQS(float animationTime)
	{
		// Positions
		glm::vec3 startPosition;
		glm::vec3 endPosition;
		if (numPositions == 1)
		{
			startPosition = endPosition = keyPositions[0].position;
		}
		else
		{
			int p0Index = GetPositionIndex(animationTime);
			int p1Index = p0Index + 1;
			startPosition = keyPositions[p0Index].position;
			endPosition = keyPositions[p1Index].position;
		}

		// Rotations
		glm::quat startRotation;
		glm::quat endRotation;
		if (numRotations == 1)
		{
			startRotation = endRotation = glm::normalize(keyRotations[0].orientation);
		}
		else
		{
			int r0Index = GetRotationIndex(animationTime);
			int r1Index = r0Index + 1;
			startRotation = glm::normalize(keyRotations[r0Index].orientation);
			endRotation = glm::normalize(keyRotations[r1Index].orientation);
		}

		// Scales
		float startScale;
		float endScale;
		if (numScales == 1)
		{
			startScale = endScale = keyScales[0].scale.x; // Assuming uniform scaling
		}
		else
		{
			int s0Index = GetScaleIndex(animationTime);
			int s1Index = s0Index + 1;
			startScale = keyScales[s0Index].scale.x; // Assuming uniform scaling
			endScale = keyScales[s1Index].scale.x;
		}

		// Single key frame fixing (Not sure why VQS breaks on this case)
		if (numPositions == 1 && numRotations == 1 && numScales == 1)
		{
			glm::mat4 translation = interpolatePosition(animationTime);
			glm::mat4 rotation = interpolateRotation(animationTime);
			glm::mat4 scale = interpolateScaling(animationTime);
			localTransform = translation * rotation * scale;
			Math::VQS vqs = Math::VQS::GLMMat4ToVQS(localTransform);
			return vqs;
		}

		// Construct the VQS for start and end transformations
		Math::Quaternion q1 = Math::Quaternion::CreateFromGLMQuat((startRotation));
		Math::Quaternion q2 = Math::Quaternion::CreateFromGLMQuat((endRotation));
		Math::VQS vqs1(startPosition, q1, startScale);
		Math::VQS vqs2(endPosition, q2, endScale);

		// Use the Interpolate method to interpolate between vqs1 and vqs2
		glm::vec3 indices = GetVQSIndices(animationTime);
		Math::VQS resultVQS = Math::VQS::Interpolate(vqs1, vqs2, indices);

		return resultVQS;
	}
#pragma endregion 

}


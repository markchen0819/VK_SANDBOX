#include "../pch.h"
#include "Transform.h"

glm::mat4 IHCEngine::Transform::Transform::GetLocalModelMatrix()
{
	glm::mat4 translationMatrix = createTranslateMatrix(localPosition);
	glm::mat4 scaleMatrix = createScaleMatrix(localScale);
	glm::mat4 rotationMatrix = createRotationMatrix(localRotation);

	localModelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
	if (parent == nullptr)
	{
		worldMatrix = localModelMatrix;
	}
	return localModelMatrix;
}

glm::mat4 IHCEngine::Transform::Transform::GetWorldMatrix()
{
	if (parent && isWorldDirty)
	{
		glm::mat4 parentMatrix = parent->GetWorldMatrix();
		glm::mat4 localTransform = GetLocalModelMatrix();
		worldMatrix = parentMatrix * localTransform;
	}
	else
	{
		worldMatrix = GetLocalModelMatrix();
	}
	isWorldDirty = false;
	return worldMatrix;
}



glm::mat4 IHCEngine::Transform::Transform::createRotationMatrix(glm::vec3 rot)
{
	return glm::eulerAngleYXZ(glm::radians(rot.y), glm::radians(rot.x), glm::radians(rot.z));	// https://glm.g-truc.net/0.9.3/api/a00164.html
}
glm::mat4 IHCEngine::Transform::Transform::createRotationMatrix(glm::quat rot)
{
	return glm::mat4_cast(rot);
}
glm::mat4 IHCEngine::Transform::Transform::createScaleMatrix(glm::vec3 scale)
{
	return glm::mat4
	{ 
		scale.x, 0, 0, 0,
		0, scale.y, 0, 0,
		0, 0, scale.z, 0,
		0, 0, 0, 1 
	};
	//glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scale);
}
glm::mat4 IHCEngine::Transform::Transform::createTranslateMatrix(glm::vec3 pos)
{
	return glm::mat4
	{ 
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		pos.x, pos.y, pos.z, 1 
	};
	// glm::translate(glm::mat4(1.0f), position);
}


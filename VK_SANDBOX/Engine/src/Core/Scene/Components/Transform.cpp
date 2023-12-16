#include "../../../pch.h"
#include "Transform.h"
#include "../GameObject.h"
#include "../Scene.h"

IHCEngine::Component::Transform::Transform()
	: Component(ComponentType::Transform),
	position(glm::vec3(0.0f)),
	rotation(glm::identity<glm::quat>()),
	scale(glm::vec3(1.0f)),
	worldModelMatrix(glm::mat4(1.0f)),
	localModelMatrix(glm::mat4(1.0f)),
	normalMatrix(glm::mat3(1.0f))
{

}

glm::mat4 IHCEngine::Component::Transform::GetModelMatrix() 
{
	return worldModelMatrix;
}

glm::mat3 IHCEngine::Component::Transform::GetNormalMatrix()
{
	return normalMatrix;
}

#pragma region Translate/ Rotate/ Scale
void IHCEngine::Component::Transform::Translate(glm::vec3 translation)
{
	position += translation;
	setDirty();
}

void IHCEngine::Component::Transform::Rotate(glm::vec3 eulerangle)
{
	// Convert the Euler angles to a quaternion
	glm::quat deltaRotation = glm::quat(glm::radians(eulerangle));
	// Apply the rotation (this avoids gimbal lock)
	rotation *= deltaRotation;
	setDirty();
}

void IHCEngine::Component::Transform::Scale(glm::vec3 scale)
{
	this->scale *= scale;
	setDirty();
}
#pragma endregion



void IHCEngine::Component::Transform::SetPosition(glm::vec3 position)
{
	this->position = position;
	setDirty();
}

void IHCEngine::Component::Transform::SetRotation(glm::vec3 angles)
{
	rotation = glm::quat(glm::radians(angles));
	rotation = glm::normalize(rotation);
	setDirty();
}

void IHCEngine::Component::Transform::SetRotationInQuaternion(glm::quat quaternion)
{
	rotation = quaternion;
	rotation = glm::normalize(rotation);
	setDirty();
}

void IHCEngine::Component::Transform::SetScale(glm::vec3 scale)
{
	this->scale = scale;
	setDirty();
}

glm::vec3 IHCEngine::Component::Transform::GetWorldPosition() const
{
	if (parent != nullptr)
	{
		glm::vec4 worldPos = parent->GetModelMatrix() * glm::vec4(position, 1.0f); // homogenous coordinates
		return glm::vec3(worldPos);
	}
	else
	{
		return position;
	}
}

glm::vec3 IHCEngine::Component::Transform::GetWorldRotation() const
{
	if (parent != nullptr)
	{
		glm::quat parentRot = parent->GetWorldRotationInQuaternion();
		glm::quat localRot = rotation;
		glm::quat worldRot = parentRot * localRot;
		return glm::degrees(glm::eulerAngles(worldRot));
	}
	return glm::degrees(glm::eulerAngles(rotation));
}

glm::quat IHCEngine::Component::Transform::GetWorldRotationInQuaternion() const
{
	if (parent != nullptr)
	{
		glm::quat parentRot = parent->GetWorldRotationInQuaternion();
		glm::quat localRot = rotation;
		return parentRot * localRot;
	}
	return rotation;
}

glm::vec3 IHCEngine::Component::Transform::GetWorldScale() const
{
	if (parent != nullptr)
	{
		return parent->GetWorldScale() * scale;
	}
	return scale;
}

void IHCEngine::Component::Transform::SetWorldPosition(glm::vec3 worldPosition)
{
	if (parent != nullptr)
	{
		// To get local position
		// Convert the target world position into parent's local space.
		glm::mat4 parentInverse = glm::inverse(parent->GetModelMatrix()); 
		glm::vec4 localPos = parentInverse * glm::vec4(worldPosition, 1.0f);
		position = glm::vec3(localPos);
	}
	else
	{
		position = worldPosition;
	}
	setDirty();
}

void IHCEngine::Component::Transform::SetWorldRotation(glm::vec3 worldAngles)
{
	SetWorldRotationInQuaternion(glm::quat(glm::radians(worldAngles)));
}

void IHCEngine::Component::Transform::SetWorldRotationInQuaternion(glm::quat worldQuaternion)
{
	if (parent != nullptr)
	{
		// conjugate (inverse for unit quaternions) of the parent's rotation with the world rotation.
		// Find target local rotation in quat (reverse parent's rotation * targetQuat)
		glm::quat localQuaternion = glm::conjugate(parent->GetRotationInQuaternion()) * worldQuaternion;
		rotation = localQuaternion;
	}
	else
	{
		rotation = worldQuaternion;
	}
	setDirty();
}

void IHCEngine::Component::Transform::SetWorldScale(glm::vec3 worldScale)
{
	if (parent != nullptr)
	{
		// Find the local scale that, when multiplied with the parent's scale, 
	   // gives the desired world scale. This is achieved by dividing 
	   // the world scale by the parent's scale
		glm::vec3 parentScale = parent->GetWorldScale();
		scale.x = parentScale.x != 0.0f ? worldScale.x / parentScale.x : worldScale.x;
		scale.y = parentScale.y != 0.0f ? worldScale.y / parentScale.y : worldScale.y;
		scale.z = parentScale.z != 0.0f ? worldScale.z / parentScale.z : worldScale.z;
	}
	else
	{
		scale = worldScale;
	}
	setDirty();
}



#pragma region Extracting the basis vectors

// right-handed systems:
// Column 0: The right vector (X-axis)
// Column 1: The up vector(Y - axis)
// Column 2 : The forward vector(Z - axis)

glm::vec3 IHCEngine::Component::Transform::GetForward()
{
	glm::vec3 scale;
	scale.x = glm::length(glm::vec3(localModelMatrix[0])); // length of the first column
	scale.y = glm::length(glm::vec3(localModelMatrix[1])); // length of the second column
	scale.z = glm::length(glm::vec3(localModelMatrix[2])); // length of the third column
	glm::mat4 model;
	model[0] = glm::vec4(glm::vec3(localModelMatrix[0]) / scale.x, localModelMatrix[0].w);
	model[1] = glm::vec4(glm::vec3(localModelMatrix[1]) / scale.y, localModelMatrix[1].w);
	model[2] = glm::vec4(glm::vec3(localModelMatrix[2]) / scale.z, localModelMatrix[2].w);
	glm::mat3 rotation = glm::mat3(model);
	return glm::normalize(glm::vec3(rotation[2]));

	// Need to remove scaling
	//return glm::normalize(glm::vec3(localModelMatrix[0][2], localModelMatrix[1][2], localModelMatrix[2][2]));
}
glm::vec3 IHCEngine::Component::Transform::GetRight()
{
	glm::vec3 scale;
	scale.x = glm::length(glm::vec3(localModelMatrix[0])); // length of the first column
	scale.y = glm::length(glm::vec3(localModelMatrix[1])); // length of the second column
	scale.z = glm::length(glm::vec3(localModelMatrix[2])); // length of the third column
	glm::mat4 model;
	model[0] = glm::vec4(glm::vec3(localModelMatrix[0]) / scale.x, localModelMatrix[0].w);
	model[1] = glm::vec4(glm::vec3(localModelMatrix[1]) / scale.y, localModelMatrix[1].w);
	model[2] = glm::vec4(glm::vec3(localModelMatrix[2]) / scale.z, localModelMatrix[2].w);
	glm::mat3 rotation = glm::mat3(model);
	return glm::normalize(glm::vec3(rotation[0]));

	// Need to remove scaling
	//return glm::normalize(glm::vec3(localModelMatrix[0][0], localModelMatrix[1][0], localModelMatrix[2][0]));
}
glm::vec3 IHCEngine::Component::Transform::GetUp()
{
	glm::vec3 scale;
	scale.x = glm::length(glm::vec3(localModelMatrix[0])); // length of the first column
	scale.y = glm::length(glm::vec3(localModelMatrix[1])); // length of the second column
	scale.z = glm::length(glm::vec3(localModelMatrix[2])); // length of the third column
	glm::mat4 model;
	model[0] = glm::vec4(glm::vec3(localModelMatrix[0]) / scale.x, localModelMatrix[0].w);
	model[1] = glm::vec4(glm::vec3(localModelMatrix[1]) / scale.y, localModelMatrix[1].w);
	model[2] = glm::vec4(glm::vec3(localModelMatrix[2]) / scale.z, localModelMatrix[2].w);
	glm::mat3 rotation = glm::mat3(model);
	return glm::normalize(glm::vec3(rotation[1]));

	// Need to remove scaling
	//return glm::normalize(glm::vec3(localModelMatrix[0][1], localModelMatrix[1][1], localModelMatrix[2][1]));
}

#pragma endregion


#pragma region SceneGraph
IHCEngine::Component::Transform IHCEngine::Component::Transform::LocalToWorld(Transform* parent, Transform* child)
{
	Transform worldTransform;
	// Calculate the world matrix.
	glm::mat4 worldMatrix = parent->GetModelMatrix() * child->GetModelMatrix();
	// Extract Position
	glm::vec3 worldPosition = glm::vec3(worldMatrix[3]);
	// Extract Scale
	glm::vec3 worldScale;
	worldScale.x = glm::length(glm::vec3(worldMatrix[0]));
	worldScale.y = glm::length(glm::vec3(worldMatrix[1]));
	worldScale.z = glm::length(glm::vec3(worldMatrix[2]));
	// Remove scaling from the matrix to extract rotation
	if (worldScale.x != 0) worldMatrix[0] /= worldScale.x;
	if (worldScale.y != 0) worldMatrix[1] /= worldScale.y;
	if (worldScale.z != 0) worldMatrix[2] /= worldScale.z;
	// Extract Rotation
	glm::quat worldRotation = glm::quat_cast(worldMatrix);
	// Set decomposed values to the worldTransform.
	worldTransform.SetPosition(worldPosition);
	worldTransform.SetRotationInQuaternion(worldRotation);
	worldTransform.SetScale(worldScale);
	return worldTransform;
}

IHCEngine::Component::Transform IHCEngine::Component::Transform::WorldToLocal(Transform* parent, Transform* world)
{
	Transform localTransform;
	// Compute parent's inverse matrix.
	glm::mat4 parentInverseMatrix = glm::inverse(parent->GetModelMatrix());
	// Calculate the local matrix.
	glm::mat4 localMatrix = parentInverseMatrix * world->GetModelMatrix();
	// Extract Position
	glm::vec3 localPosition = glm::vec3(localMatrix[3]);
	// Extract Scale
	glm::vec3 localScale;
	localScale.x = glm::length(glm::vec3(localMatrix[0]));
	localScale.y = glm::length(glm::vec3(localMatrix[1]));
	localScale.z = glm::length(glm::vec3(localMatrix[2]));
	// Remove scaling from the matrix to extract rotation
	if (localScale.x != 0) localMatrix[0] /= localScale.x;
	if (localScale.y != 0) localMatrix[1] /= localScale.y;
	if (localScale.z != 0) localMatrix[2] /= localScale.z;
	// Extract Rotation
	glm::quat localRotation = glm::quat_cast(localMatrix);
	// Set decomposed values to the localTransform.
	localTransform.SetPosition(localPosition);
	localTransform.SetRotationInQuaternion(localRotation);
	localTransform.SetScale(localScale);
	return localTransform;
}

void IHCEngine::Component::Transform::SetParent(Transform* parentTransform)
{
	if (parent != nullptr) // Already has a parent
	{
		// Remove this from its current parent's children list
		auto it = std::find(parent->children.begin(), parent->children.end(), this);
		if (it != parent->children.end())
		{
			parent->children.erase(it);
		}
	}
	// New Parent
	parent = parentTransform;
	if (parentTransform != nullptr)
	{
		parentTransform->children.push_back(this);
	}
	setDirty(); 
}

IHCEngine::Component::Transform* IHCEngine::Component::Transform::RemoveParent()
{
	Transform* oldParent = parent;
	if (parent != nullptr)
	{
		// Remove this from its current parent's children list
		auto it = std::find(parent->children.begin(), parent->children.end(), this);
		if (it != parent->children.end())
		{
			parent->children.erase(it);
		}
		parent = nullptr;
		setDirty(); 
	}
	return oldParent;
}

IHCEngine::Component::Transform* IHCEngine::Component::Transform::GetParent()
{
	return parent;
}

void IHCEngine::Component::Transform::SetChild(Transform* childTransform)
{
	if (childTransform != nullptr)
	{
		childTransform->SetParent(this);
	}
}

IHCEngine::Component::Transform* IHCEngine::Component::Transform::GetChildAt(int index)
{
	if (index > children.size())
	{
		assert("Transform does not have child at index");
	}
	return children[index];
}

bool IHCEngine::Component::Transform::IsChildOf(Transform* parent)
{
	return this->parent == parent;
}
int IHCEngine::Component::Transform::GetChildCount()
{
	return children.size();
}

void IHCEngine::Component::Transform::Propagate()
{
	if (isDirty)
	{
		// Compute local model matrix first
		glm::mat4 transMat = glm::translate(glm::mat4(1.0f), position);
		glm::mat4 rotMat = glm::mat4_cast(rotation);  // Convert quaternion to rotation matrix
		glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);
		localModelMatrix = transMat * rotMat * scaleMat;

		// Compute normal matrix
		if (scale.x == scale.y && scale.y == scale.z) // uniform scaling
		{
			normalMatrix = glm::mat3(localModelMatrix);
		}
		else
		{
			normalMatrix = glm::transpose(glm::inverse(localModelMatrix));
		}
	}

	// Compute world matrix
	if (parent)
	{
		worldModelMatrix = parent->worldModelMatrix * localModelMatrix;
	}
	else
	{
		worldModelMatrix = localModelMatrix;
	}

	// Reset the dirty flag for this node
	isDirty = false;


	// Now propagate to children
	for (int i = 0; i < GetChildCount(); ++i)
	{
		IHCEngine::Component::Transform* childTransform = GetChildAt(i);
		childTransform->Propagate();
	}
}


#pragma endregion

void IHCEngine::Component::Transform::Attach()
{
}

void IHCEngine::Component::Transform::Remove()
{
}


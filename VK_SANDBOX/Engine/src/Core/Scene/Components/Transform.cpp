#include "../../../pch.h"
#include "Transform.h"
#include "../GameObject.h"
#include "../Scene.h"

IHCEngine::Component::Transform::Transform()
	: Component(ComponentType::Transform),
	position(glm::vec3(0.0f)),
	rotation(glm::vec3(0.0f)),
	scale(glm::vec3(1.0f)),
	worldModelMatrix(glm::mat4(1.0f)),
	localModelMatrix(glm::mat4(1.0f)),
	normalMatrix(glm::mat3(1.0f)),
	rotationQuaternion(glm::identity<glm::quat>())
{

}

glm::mat4 IHCEngine::Component::Transform::GetModelMatrix() 
{
	if (isDirty || (parent != nullptr && parent->IsDirty()))
	{
		// update model matrix
		glm::mat4 transMat = glm::translate(glm::mat4(1.0f), position);
		//glm::mat4 rotMat = glm::eulerAngleXYZ(glm::radians(rotation.x),
		//									  glm::radians(rotation.y),
		//								      glm::radians(rotation.z));
		glm::mat4 rotMat = glm::mat4_cast(rotationQuaternion);  // Convert quaternion to rotation matrix
		glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);
		localModelMatrix = transMat * rotMat * scaleMat;

		// update normal matrix
		if (scale.x == scale.y && scale.y == scale.z) // uniform scaling
		{
			normalMatrix = glm::mat3(localModelMatrix);
		}
		else
		{
			normalMatrix = glm::transpose(glm::inverse(localModelMatrix));
		}


		// SCENE GRAPH!!
		// If there's a parent, multiply with parent's model matrix
		// This approach will traverse up the hierarchy 
		// every time GetModelMatrix is called, making it 
		// inefficient for deep hierarchies
		if (parent != nullptr)
		{
			worldModelMatrix = parent->GetModelMatrix() * localModelMatrix;
		}
		else
		{
			worldModelMatrix = localModelMatrix;
		}
		isDirty = false;
	}
	return worldModelMatrix;
}

glm::mat3 IHCEngine::Component::Transform::GetNormalMatrix()
{
	// values updated after calling GetModelMatrix()
	GetModelMatrix();
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
	rotation += eulerangle;
	rotation.x = std::fmod(rotation.x + 360.0f, 360.0f);
	rotation.y = std::fmod(rotation.y + 360.0f, 360.0f);
	rotation.z = std::fmod(rotation.z + 360.0f, 360.0f);

	// Quaternioin
	glm::quat pitch = glm::angleAxis(glm::radians(eulerangle.x), glm::vec3(1, 0, 0));
	glm::quat yaw = glm::angleAxis(glm::radians(eulerangle.y), glm::vec3(0, 1, 0));
	glm::quat roll = glm::angleAxis(glm::radians(eulerangle.z), glm::vec3(0, 0, 1));
	// Multiply quaternions in the correct order
	rotationQuaternion = yaw * pitch * roll * rotationQuaternion;

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
	rotationQuaternion = glm::identity<glm::quat>();
	rotation = glm::vec3(0.0f);
	Rotate(angles);
	////setDirty(); // called already in Rotate(angles)
}

void IHCEngine::Component::Transform::SetRotationInQuaternion(glm::quat quaternion)
{
	rotationQuaternion = quaternion;
	setDirty();
	// below is display only for now
	rotation = glm::degrees(glm::eulerAngles(quaternion));
	//glm::vec3 eulerDegrees = glm::degrees(glm::eulerAngles(quaternion));
	//SetRotation(eulerDegrees);
	////setDirty(); // called already in Rotate(angles)
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
		glm::quat parentRot = glm::quat(glm::radians(parent->GetWorldRotation()));
		glm::quat localRot = glm::quat(glm::radians(rotation));
		glm::quat worldRot = parentRot * localRot;
		return glm::degrees(glm::eulerAngles(worldRot));
	}
	return rotation;
}

glm::quat IHCEngine::Component::Transform::GetWorldRotationInQuaternion() const
{
	if (parent != nullptr)
	{
		glm::quat parentRot = glm::quat(glm::radians(parent->GetWorldRotation()));
		glm::quat localRot = glm::quat(glm::radians(rotation));
		return parentRot * localRot;
	}
	return glm::quat(glm::radians(rotation));
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
		rotation = glm::degrees(glm::eulerAngles(localQuaternion));
	}
	else
	{
		rotation = glm::degrees(glm::eulerAngles(worldQuaternion));
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
	return glm::normalize(glm::vec3(localModelMatrix[0][2], localModelMatrix[1][2], localModelMatrix[2][2]));
}
glm::vec3 IHCEngine::Component::Transform::GetRight()
{
	return glm::normalize(glm::vec3(localModelMatrix[0][0], localModelMatrix[1][0], localModelMatrix[2][0]));
}
glm::vec3 IHCEngine::Component::Transform::GetUp()
{
	return glm::normalize(glm::vec3(localModelMatrix[0][1], localModelMatrix[1][1], localModelMatrix[2][1]));
}

#pragma endregion



IHCEngine::Component::Transform IHCEngine::Component::Transform::LocalToWorld(Transform* parent, Transform* child)
{
	Transform result;

	result.SetScale(parent->GetScale() * child->GetScale());

    // Rotations are stored as Euler angles
	// Convert degrees to radians then to Quaternion
	glm::quat parentQuat = glm::quat(glm::radians(parent->GetRotation())); 
	glm::quat childQuat = glm::quat(glm::radians(child->GetRotation()));
	glm::quat resultQuat = parentQuat * childQuat;
	// Convert back to degrees
	result.SetRotation(glm::degrees(glm::eulerAngles(resultQuat)));  

	result.SetPosition(parent->GetPosition() + parent->GetScale() * child->GetPosition());

	return result;
}

IHCEngine::Component::Transform IHCEngine::Component::Transform::WorldToLocal(Transform* parent, Transform* world)
{
	Transform result;
	result.SetScale(world->GetScale() / parent->GetScale());
	result.SetRotation(world->GetRotation() - parent->GetRotation());
	result.SetPosition((world->GetPosition() - parent->GetPosition()) / parent->GetScale());
	return result;
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
	//this->parentLocalMatrix = parentLocalTransform;
	//for (const auto& childTransform : this->children)
	//{
	//	childTransform->PropagateParentLocalTransform(parentLocalTransform * this->GetLocalModelMatrix(true));
	//}
	//setWorldDirty();
	setDirty();
	for (const auto& childTransform : this->children)
	{
		childTransform->GetModelMatrix();
	}

}













#pragma region Local

#pragma endregion


#pragma region World

#pragma endregion


#pragma region World and LocalModelMatrix

#pragma endregion



#pragma region SceneGraph Hierachy

#pragma endregion

void IHCEngine::Component::Transform::Remove()
{
}


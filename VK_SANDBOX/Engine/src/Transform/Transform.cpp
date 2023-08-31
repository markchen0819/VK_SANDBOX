#include "../pch.h"
#include "Transform.h"

IHCEngine::Transform::Transform::Transform()
	:localModelMatrix(1.0f),
	 localPosition(0.0),
	 localRotation(1.0, 0.0, 0.0, 0.0),
	 localScale(1.0f),
	 worldMatrix(1.0f),
	 worldPosition(0.0),
	 worldScaleOffset(1.0f),
	 parent(nullptr),
	 parentLocalMatrix(1.0f),
	 inverseOfOriginalParentLocalModelMatrix(1.0f)
{ 

}

#pragma region Translate/ Rotate/ Scale
void IHCEngine::Transform::Transform::Translate(glm::vec3 translation, Space space)
{
	if (parent == nullptr || space == Space::Local)
	{
		localPosition += translation;
		setLocalDirty();
	}
	else if (space == Space::World)
	{
		worldPosition += translation;
		setWorldDirty();
	}
}
void IHCEngine::Transform::Transform::Rotate(glm::vec3 eulers, Space space)
{
	if (parent == nullptr || space == Space::Local)
	{
		localRotation = glm::quat(eulers) * localRotation;
		// GLM quaternions are automatically normalized, just explicit call again
		localRotation = glm::normalize(localRotation);
		setLocalDirty();

	}
	else if (space == Space::World)
	{
		worldRotationOffset = glm::quat(eulers) * worldRotationOffset;
		setWorldDirty();
	}
}
void IHCEngine::Transform::Transform::Scale(glm::vec3 scale, Space space)
{
	if (parent == nullptr || space == Space::Local)
	{
		localScale = scale;
		setLocalDirty();
	}
	else if (space == Space::World)
	{
		worldScaleOffset = scale;
		setWorldDirty();
	}
}
#pragma endregion

#pragma region Local
glm::vec3 IHCEngine::Transform::Transform::GetLocalPosition()
{
	return localPosition;
}
void IHCEngine::Transform::Transform::SetLocalPosition(glm::vec3 position)
{
	localPosition = position;
	setLocalDirty();
}
glm::vec3 ToEulerAngles(glm::quat& q)
{
	glm::vec3 angles;

	// Roll (x-axis rotation)
	float sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
	float cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
	angles.x = std::atan2(sinr_cosp, cosr_cosp);

	// Pitch (y-axis rotation)
	float sinp = 2 * (q.w * q.y - q.z * q.x);
	if (std::abs(sinp) >= 1)
		angles.y = std::copysign(glm::pi<float>() / 2, sinp); // Use 90 degrees if out of range
	else
		angles.y = std::asin(sinp);

	// Yaw (z-axis rotation)
	float siny_cosp = 2 * (q.w * q.z + q.x * q.y);
	float cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
	angles.z = std::atan2(siny_cosp, cosy_cosp);

	return angles;
}
glm::vec3 IHCEngine::Transform::Transform::GetLocalEulerAngles()
{
	return ToEulerAngles(localRotation);
}
void IHCEngine::Transform::Transform::SetLocalEulerAngles(glm::vec3 eulers)
{
	localRotation = glm::quat(eulers);
	setLocalDirty();
}
glm::vec3 IHCEngine::Transform::Transform::GetLocalScale()
{
	return localScale;
}
void IHCEngine::Transform::Transform::SetLocalScale(glm::vec3 scale)
{
	localScale = scale;
	setLocalDirty();
}
#pragma endregion


#pragma region World
glm::vec3 IHCEngine::Transform::Transform::GetWorldPosition()
{   
	// | x1 y1 z1 tx |
	// | x2 y2 z2 ty |
    // | x3 y3 z3 tz |
    // |  0  0  0  1 |

	return GetWorldMatrix()[3];
}
void IHCEngine::Transform::Transform::SetWorldPosition(glm::vec3 position)
{
	if (parent != nullptr)
	{
		// Offset between the desired world position and the current position
		glm::vec3 worldOffset = position - GetWorldPosition();
		// Extract the rotation matrix from the world transformation matrix
		glm::mat3 worldRotationMatrix = glm::mat3(GetWorldMatrix());
		// Invert the local model's rotation matrix
		glm::mat3 invLocalRotationMatrix = glm::inverse(glm::mat3(localModelMatrix));
		// Apply the rotation matrices to the world offset to align it with the local axes
		glm::vec3 localOffset = worldRotationMatrix * invLocalRotationMatrix * worldOffset;
		localPosition += localOffset;
		// ex:
		// Premise:
		// toy car at center of table pointing (0,1)
		// place target flag at (10,0)  -> (worldOffset)
		// rotate car pointing (1,0) (local forward direction is now aligned with the world's east direction)
		// Task :
		// Move the car to target using local controls
		// If car not rotated (pointing (0,1)) need to move sideways (10,0) -> (invLocalRotationMatrix * worldOffset)
		// If rotated, move forward by 10 -> (worldRotationMatrix * invLocalRotationMatrix * worldOffset)
	}
	else
	{
		localPosition = position;
	}
	isLocalDirty = true;
}
// Converts a non-orthogonal 3x3 matrix to a quaternion.
// This function takes into account potential scaling and skewing present in the matrix.
glm::quat SetNonOrthogonalRotationMatrix(const glm::mat3& matrix)
{
	// Extract matrix elements
	float m00 = matrix[0][0];
	float m11 = matrix[1][1];
	float m22 = matrix[2][2];

	// Compute matrix determinant raised to the power of 1/3.
	// This step helps factor out uniform scaling.
	float absQ2 = std::pow(glm::determinant(matrix), 1.0f / 3.0f);  // Address scaling

	// Compute the quaternion components
	float w = std::sqrt(std::max(0.0f, absQ2 + m00 + m11 + m22)) / 2.0f;
	float x = std::sqrt(std::max(0.0f, absQ2 + m00 - m11 - m22)) / 2.0f;
	float y = std::sqrt(std::max(0.0f, absQ2 - m00 + m11 - m22)) / 2.0f;
	float z = std::sqrt(std::max(0.0f, absQ2 - m00 - m11 + m22)) / 2.0f;

	// Adjust quaternion component signs based on matrix elements.
	// This step helps ensure the correct orientation, particularly in the presence of skew.
	x = std::copysign(x, matrix[2][1] - matrix[1][2]);  // Address skewing
	y = std::copysign(y, matrix[0][2] - matrix[2][0]);  // Address skewing
	z = std::copysign(z, matrix[1][0] - matrix[0][1]);  // Address skewing

	// Return constructed quaternion
	return glm::quat(w, x, y, z);
}
glm::quat IHCEngine::Transform::Transform::GetWorldRotation()
{
	// | x1 y1 z1 tx |       | x1 y1 z1 |
    // | x2 y2 z2 ty |  ->   | x2 y2 z2 |  -> SetNonOrthogonalRotationMatrix -> Quarternion
	// | x3 y3 z3 tz |       | x3 y3 z3 |
	// |  0  0  0  1 |

	// Extract 3x3 rotation matrix
	glm::mat3 rotationMatrix = glm::mat3(GetWorldMatrix());
	// Convert the rotational matrix to a quaternion using our specialized function
	return SetNonOrthogonalRotationMatrix(rotationMatrix);
}
void IHCEngine::Transform::Transform::SetWorldRotation(glm::quat rotation)
{
	if (parent == nullptr)
	{
		// When there's no parent, the world rotation offset is set to the given rotation
		worldRotationOffset = rotation;
		glm::normalize(worldRotationOffset);
		isWorldDirty = true;
		return;
	}
	// Normalize and set the local rotation for when there is a parent
	localRotation = glm::normalize(rotation);
	isLocalDirty = true;
}
#pragma endregion


#pragma region World and LocalModelMatrix
glm::mat4 IHCEngine::Transform::Transform::GetLocalModelMatrix(bool overrideDirtyFlag)
{
	if (isLocalDirty || overrideDirtyFlag)
	{
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), localPosition);
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), localScale);
		glm::mat4 rotationMatrix = glm::mat4_cast(localRotation);
		localModelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
		if (parent == nullptr)
		{
			worldMatrix = localModelMatrix;
		}
	}
	return localModelMatrix;
}
glm::mat4 IHCEngine::Transform::Transform::GetWorldMatrix()
{
	if (isWorldDirty || isLocalDirty)
	{
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), worldPosition);
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), worldScaleOffset);

		if (parent != nullptr)
		{
			// (ex: starting at platform world (1,0,0) and player world (3,0,0) local (2,0,0))
			
			// Child is first detached from its original position (2,0,0) -> (0,0,0)
			// then attached to the parent's current position, (6,0,0)
			// and finally applies its own transformation. (8,0,0)
			// (ex: platform move 5 to right (6,0,0)
			//      player world (8,0,0) local(2,0,0) (repect to platform)
			glm::mat4 combinedTransform = parentLocalMatrix * inverseOfOriginalParentLocalModelMatrix * GetLocalModelMatrix();
			
			// Extracts the translation part of the combined transformation. 
			// The child's offset from the parent.
			// (ex: player plants a flag uses as a reference point for his next moves.
			//      flag planted at his current location, world (8, 0, 0))
			glm::vec3 offsetFromParent = glm::vec3(combinedTransform[3]);
			
			// Creating a matrix just for this offset. 
			// This helps in moving the child without any rotation or scaling.
			glm::mat4 offsetMatrix = glm::translate(glm::mat4(1.0f), offsetFromParent);

			// (ex: For translationMatrix
			//      player decides to move forward in the world 
			//      to inspect the surroundings local (3,0,0) world (11,0,0)

			// Additional world rotation applied to the child.
			// (ex: decides to rotate 90 to his left to face a specific landmark)
			glm::mat4 worldRotationMatrix = glm::mat4_cast(worldRotationOffset);

			// (ex: For caleMatrix grow 2 times)

			// Construct the final world matrix for the child.
			glm::mat4 worldMatrix = offsetMatrix   // Step 1: Reference point. (translate to world (8,0,0) local (0,0,0))
				* translationMatrix                // Step 2: Movement in the world. (world(11,0,0) local(3,0,0))
				* worldRotationMatrix              // Step 3: Rotation. (rotates toward landmark)
				* scaleMatrix                      // Step 4: Scaling. (grow 2 times)
				* glm::inverse(offsetMatrix)       // Step 5: Reverting reference point. 
				// (world still at (11,0,0), considering platform instead of flag as referece 
				// player's local position with respect to the flag 
				// is now considered to be the player's world position
				// -> local (11,0,0))
				* combinedTransform;               // Step 6: Considering initial relative movement. 
			    // (adjust everything according to when player was on moving platform
				//  apply (2,0,0) (reaffirming the player's initial relationship), 
			    // remains world(11,0,0) local(2,0,0)
		}
		else
		{
			worldMatrix = GetLocalModelMatrix() * translationMatrix * glm::mat4_cast(worldRotationOffset) * scaleMatrix;
		}

		isWorldDirty = false;
		isLocalDirty = false;
	}

	return worldMatrix;
}
#pragma endregion

#pragma region Extracting the basis vectors
// right-handed systems:
// Column 0: The right vector (X-axis)
// Column 1: The up vector(Y - axis)
// Column 2 : The forward vector(Z - axis)
glm::vec3 IHCEngine::Transform::Transform::GetForward()
{
	return glm::mat3(GetWorldRotation())[2];
}
glm::vec3 IHCEngine::Transform::Transform::GetRight()
{
	return glm::mat3(GetWorldRotation())[0];
}
glm::vec3 IHCEngine::Transform::Transform::GetUp()
{
	return glm::mat3(GetWorldRotation())[1];
}
#pragma endregion


#pragma region SceneGraph Hierachy
IHCEngine::Transform::Transform* IHCEngine::Transform::Transform::GetParent()
{
	return parent;
}
void IHCEngine::Transform::Transform::SetParent(Transform* parentTransform)
{
	if (parentTransform != nullptr && parentTransform != parent)
	{
		parent = parentTransform;  // Set the new parent
		parent->SetChild(this);
		// Store the inverse of the parent's world matrix at the time of parenting.
		// This can be useful for calculations where you want to revert or compare
		// against the original transformation state at the time of setting the parent.
		inverseOfOriginalParentLocalModelMatrix = glm::inverse(parent->GetWorldMatrix());
		// Update all children of the new parent to ensure they are aware of the new transformation hierarchy
		for (const auto& childTransform : parentTransform->children)
		{
			childTransform->PropagateParentLocalTransform(parent->GetLocalModelMatrix());
		}
		// Notify the scene that something changed, so it might need to update/rebuild certain data structures or caches.
		//gameObject->GetScene()->SetDirtyFlag();
	}
}
IHCEngine::Transform::Transform* IHCEngine::Transform::Transform::RemoveParent()
{
	Transform* parentBeingRemoved = nullptr;
	if (parent != nullptr)
	{
		// Save a reference to the parent that is being removed.
		parentBeingRemoved = parent;
		// Find this object in the parent's list of children
		auto it = std::find(parent->children.begin(), parent->children.end(), this);
		// Remove this object from the parent's list of children
		if (it != parent->children.end()) 
		{
			parent->children.erase(it);
		}
		parent = nullptr;

		// Since the parent is being removed, 
		// the child's world transformations now become its local transformations.
		// save the current rotation, scale, and position
		
		localPosition = glm::vec3(worldMatrix[3]);
		localScale = glm::vec3(glm::length(worldMatrix[0]), glm::length(worldMatrix[1]), glm::length(worldMatrix[2]));
		glm::mat3 rotationMatrix = glm::mat3(worldMatrix); // Extracting 3x3 rotation matrix
		localRotation = glm::quat(rotationMatrix); // Convert rotation matrix to quaternion
		localRotation = glm::normalize(localRotation); // Normalize the quaternion
		isLocalDirty = true;

		// Reset world data
		worldPosition = glm::vec3(0.0f);
		worldScaleOffset = glm::vec3(1.0f);
		worldRotationOffset = glm::quat(1.0, 0.0, 0.0, 0.0); // Identity quaternion
		parentLocalMatrix = glm::mat4(1.0f); // Identity matrix
		inverseOfOriginalParentLocalModelMatrix = glm::mat4(1.0f); 
		// Notify the scene that something changed, so it might need to update/rebuild certain data structures or caches.
		//gameObject->GetScene()->SetDirtyFlag();
	}
	return parentBeingRemoved;
}
void IHCEngine::Transform::Transform::SetChild(Transform* childTransform)
{
	children.push_back(childTransform);
}
bool IHCEngine::Transform::Transform::IsChildOf(Transform* parent)
{
	return this->parent == parent;
}
void IHCEngine::Transform::Transform::PropagateParentLocalTransform(glm::mat4 parentLocalTransform)
{
	this->parentLocalMatrix = parentLocalTransform;
	for (const auto& childTransform : this->children)
	{
		childTransform->PropagateParentLocalTransform(parentLocalTransform * this->GetLocalModelMatrix(true));
	}
	setWorldDirty();
}
#pragma endregion
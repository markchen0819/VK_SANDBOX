#pragma once

namespace IHCEngine::Transform
{
	class Transform
	{
	public:

		//Transform();
		//Transform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
		//Transform(const Transform& t);
		//~Transform();
		//void operator=(const Transform& t);

		// Local
		glm::vec3 GetLocalPosition() { localPosition; };
		glm::vec3 GetLocalScale() { localScale; };
		glm::vec3 GetRotation() { localRotation; };
		void SetLocalPosition(glm::vec3 pos)
		{
			localPosition = pos;
			setWorldDirty();
		};
		void SetLocalScale(glm::vec3 scale)
		{
			localScale = scale;
			setWorldDirty();
		}
		void SetLocalRotation (glm::quat& rot) 
		{
			localRotation = rot;
			setWorldDirty();
		};
		glm::mat4 GetLocalModelMatrix();

		// World
		glm::vec3 GetWorldPosition()
		{
			if (parent && isWorldDirty)
			{
				// Considering all parent transformations.
				glm::mat4 parentMatrix = parent->GetWorldMatrix();
				glm::vec4 worldPosition4 = parentMatrix * glm::vec4(localPosition, 1.0f);
				worldPosition = glm::vec3(worldPosition4);
			}
			else 
			{
				worldPosition = localPosition;
			}
			isWorldDirty = false;
			return worldPosition;
		}
		glm::vec3 GetWorldScale() 
		{
			if (parent && isWorldDirty)
			{
				// When combining scales, it's simply multiplication
				worldScale = parent->GetWorldScale() * localScale;			
			}
			else 
			{
				worldScale = localScale;
			}
			isWorldDirty = false;
			return worldScale;
		}
		glm::quat GetWorldRotation()
		{
			if (parent && isWorldDirty) 
			{
				// For rotations, you multiply the quaternions
				worldRotation = parent->GetWorldRotation() * localRotation;
			}
			else 
			{
				worldRotation = localRotation;
			}
			isWorldDirty = false;
			return worldRotation;
		}
		glm::mat4 GetWorldMatrix();

		// Parent
		void SetParent(Transform* newParent) 
		{
			parent = newParent;
			setWorldDirty();
		}
		Transform* GetParent() const
		{
			return parent;
		}

	private:

		Transform* parent = nullptr;

		glm::mat4 localModelMatrix;
		glm::vec3 localPosition;
		glm::quat localRotation;
		glm::vec3 localScale;

		glm::mat4 worldMatrix;
		glm::vec3 worldPosition;
		glm::quat worldRotation;
		glm::vec3 worldScale;

		bool isWorldDirty = true;
		void setWorldDirty() 
		{ isWorldDirty = true; }

		glm::mat4 createRotationMatrix(glm::vec3 rot); // EulerAngles
		glm::mat4 createRotationMatrix(glm::quat rot); // Queternion
		glm::mat4 createScaleMatrix(glm::vec3 scale);
		glm::mat4 createTranslateMatrix(glm::vec3 pos);

	};
}


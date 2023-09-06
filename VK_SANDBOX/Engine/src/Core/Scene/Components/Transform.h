#pragma once
#include <glm/ext/quaternion_float.hpp>
#include "Component.h"

// Forward declaration
namespace IHCEngine::Core
{
	class GameObject;
}

namespace IHCEngine::Component
{
	enum class Space
	{
		Local,
		World
	};

	class Transform : public Component
	{
	public:

		Transform();

		glm::mat4 GetModelMatrix();
		glm::mat3 GetNormalMatrix();

		// Local

		void Translate(glm::vec3 translation);
		void Rotate(glm::vec3 eulerangle);
		void Scale(glm::vec3 scale);

		glm::vec3 GetForward();
		glm::vec3 GetRight();
		glm::vec3 GetUp();

		glm::vec3 GetPosition() const { return position; };
		glm::vec3 GetRotation() const { return rotation; }; // Euler angles
		glm::quat GetRotationInQuaternion() const { return glm::quat(glm::radians(rotation)); }
		glm::vec3 GetScale() const { return scale; };
		void SetPosition(glm::vec3 position);
		void SetRotation(glm::vec3 angles);
		void SetRotationInQuaternion(glm::quat quaternion);
		void SetScale(glm::vec3 scale);

		// Global
		glm::vec3 GetWorldPosition() const;
		glm::vec3 GetWorldRotation() const; // Euler angles
		glm::quat GetWorldRotationInQuaternion() const;
		glm::vec3 GetWorldScale() const;
		void SetWorldPosition(glm::vec3 worldPosition);
		void SetWorldRotation(glm::vec3 worldAngles);
		void SetWorldRotationInQuaternion(glm::quat worldQuaternion);
		void SetWorldScale(glm::vec3 worldScale);


		// computes result of local to world (become parent)
		Transform LocalToWorld(Transform* parent, Transform* child);
		// computes result of world to local (become child)
		Transform WorldToLocal(Transform* parent, Transform* world);

		void SetParent(Transform* parentTransform);
		Transform* RemoveParent();
		Transform* GetParent();

		void SetChild(Transform* childTransform);
		Transform* GetChildAt(int index);
		bool IsChildOf(Transform* parent);
		int GetChildCount();

		// scenegraph
		//void PropagateParentLocalTransform(glm::mat4 parentLocalTransform);
		void Propagate();
		bool IsDirty() { return isDirty; }
	private:

		Transform* parent = nullptr;
		std::vector<Transform*> children;

		// Globals are calculated
		glm::mat4 worldModelMatrix; 

		// Locals are where values are stored/ set
		glm::mat3 normalMatrix;
		glm::vec3 position;
		glm::vec3 rotation; // store in degrees instead of radians
		glm::vec3 scale;
		glm::mat4 localModelMatrix;

		glm::quat rotationQuaternion;

		bool isDirty = false;
		void setDirty() { isDirty = true; }

		void Remove() override;
	};
}


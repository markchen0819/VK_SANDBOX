#pragma once

// Forward declaration
namespace IHCEngine::Core
{
	class GameObject;
}

namespace IHCEngine::Transform
{
	enum class Space
	{
		Local,
		World
	};

	class Transform
	{
	public:

		Transform();

		void Translate(glm::vec3 translation, Space space);
		void Rotate(glm::vec3 eulers, Space space);
		void Scale(glm::vec3 scale, Space space);


		glm::vec3 GetLocalPosition();
		void SetLocalPosition(glm::vec3  position);
		glm::vec3 GetLocalEulerAngles();
		void SetLocalEulerAngles(glm::vec3 eulers);
		glm::vec3 GetLocalScale();
		void SetLocalScale(glm::vec3 scale);


		glm::vec3 GetWorldPosition();
		void SetWorldPosition(glm::vec3 position);
		glm::quat GetWorldRotation();
		void SetWorldRotation(glm::quat rotation);


		glm::mat4 GetLocalModelMatrix(bool overrideDirtyFlag = false);
		glm::mat4 GetWorldMatrix();


		glm::vec3 GetForward();
		glm::vec3 GetRight();
		glm::vec3 GetUp();


		Transform* GetParent();
		void SetParent(Transform* parentTransform);
		Transform* RemoveParent();// applies extra calculation
		void SetChild(Transform* childTransform);
		bool IsChildOf(Transform* parent);


	private:

		IHCEngine::Core::GameObject* gameObject;
		Transform* parent = nullptr;
		std::vector<Transform*> children;


		glm::mat4 localModelMatrix;
		glm::vec3 localPosition;
		glm::quat localRotation;
		glm::vec3 localScale;

		glm::mat4 worldMatrix;
		glm::vec3 worldPosition;
		glm::quat worldRotationOffset;
		glm::vec3 worldScaleOffset;
		
		glm::mat4 parentLocalMatrix;
		// OriginalParentLocalModelMatrix: 
		// the initial local transformation of the parent when the child was attached
		// Inverse of this matrix:
		// transformation that "undoes" the parent's original transformation
		glm::mat4 inverseOfOriginalParentLocalModelMatrix;

		bool isLocalDirty = false;
		void setLocalDirty()
		{ isLocalDirty = true;}
		bool isWorldDirty = false;
		void setWorldDirty() 
		{ isWorldDirty = true; }

		// scenegraph
		void PropagateParentLocalTransform(glm::mat4 parentLocalTransform);
	};
}


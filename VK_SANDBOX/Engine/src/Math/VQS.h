#pragma once
#include "Quaternion.h"

namespace IHCEngine::Math
{
	class VQS
	{
	public:

		VQS();
		VQS(glm::vec3 const& translate, Quaternion const& quaternion, float scalar = 1.0f);
		~VQS() = default;

		// Getters
		glm::vec3 GetTranslate() const { return translate; }
		Quaternion GetRotation() const { return rotation; }
		float GetScalar() const { return scalar; }

		// Operations
		VQS operator+(VQS const& rhs) const;
		VQS operator*(VQS const& rhs) const;
		VQS operator*(float const& scalar) const;
		glm::vec3 operator*(glm::vec3 const& point) const;
		VQS Inverse() const;

		static VQS Slerp(VQS const& vqs1, VQS const& vqs2, glm::vec3 t); //glm::vec3 t as keyframes might not aligned
		static glm::mat4 VQSToGLMMat4(VQS const& vqs);
		static VQS GLMMat4ToVQS(glm::mat4 const& mat);

	private:

		glm::vec3 translate;
		Quaternion rotation;
		float scalar;

	};

}
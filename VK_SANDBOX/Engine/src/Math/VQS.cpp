#include "../pch.h"
#include "MathConstants.h"
#include "VQS.h"

namespace IHCEngine::Math
{
	VQS::VQS()
		:	translate(glm::vec3(0.0f)),
			rotation(Quaternion()),
			scalar(1.0f)
	{}

	VQS::VQS(glm::vec3 const& translate, Quaternion const& quaternion, float scalar)
		:	translate(translate),
			rotation(quaternion),
			scalar(scalar)
	{}



	VQS VQS::operator+(VQS const& rhs) const
	{
		VQS vqs;
		vqs.scalar = this->scalar + rhs.scalar;
		vqs.rotation = this->rotation + rhs.rotation;
		vqs.translate = this->translate + rhs.translate;
		return vqs;
	}

	VQS VQS::operator*(float const& scalar) const
	{
		VQS vqs;
		vqs.scalar *= scalar;
		vqs.rotation = vqs.rotation * scalar;
		vqs.translate *= scalar;
		return vqs;
	}


	glm::vec3 VQS::operator*(glm::vec3 const& point) const
	{
		// Tr = [v,q,s]r = q(sr)q^-1 + v
		// r
		glm::vec3 scaledPoint = point * scalar;
		// sr
		Quaternion pointAsQuaternion(0, scaledPoint);
		// q^-1
		Quaternion inverseRotation = Quaternion::Inverse(rotation);
		// q(sr)q^-1
		Quaternion rotatedPointQuat = rotation * pointAsQuaternion * inverseRotation;
		// q(sr)q^-1 + v
		glm::vec3 transformedPoint = rotatedPointQuat.GetImaginary() + this->translate;
		return transformedPoint;
	}

	VQS VQS::operator*(VQS const& rhs) const
	{
		// T2T1 = [T2v1, q2q1, s2s1]  
		// Apply the current VQS transformation to the translation of the other VQS.
		glm::vec3 combinedTranslation = *this * rhs.translate;
		// q2q1
		Quaternion combinedRotation = rotation * rhs.rotation;
		// s2s1
		float combinedScalar = scalar * rhs.scalar;
		return VQS(combinedTranslation, combinedRotation, combinedScalar);
	}

	VQS VQS::Inverse() const
	{
		// T^-1 = [q^-1 (s^-1 v^-1) q, q^-1, s^-1]
		float inverseScalar = 1.0f / scalar;
		glm::vec3 inverseTranslationVector = -translate;
		glm::vec3 scaledInverseTranslation = inverseTranslationVector * inverseScalar;
		// (s^-1 v^-1) in quaternion representation.
		Quaternion scaledInverseTranslationQuat(0, scaledInverseTranslation);
		// q^-1
		Quaternion inverseRotation = Quaternion::Inverse(rotation);
		// q^-1 (s^-1 v^-1) q 
		Quaternion rotatedInverseTranslationQuat = 
			inverseRotation
			* scaledInverseTranslationQuat
			* rotation;
		// Extract the vector part of the rotated inverse translation quaternion to get the final translation component.
		glm::vec3 finalInverseTranslation = rotatedInverseTranslationQuat.GetImaginary();
		// Construct the inverse VQS 
		return VQS(finalInverseTranslation, inverseRotation, inverseScalar);
	}

	VQS VQS::Interpolate(VQS const& vqs1, VQS const& vqs2, glm::vec3 t)
	{
		// vt = Lerp(v0,v1,t)
		// qt = Slerp(q0, q1, t)
		// st = Elerp(s0, s1, t)

		glm::vec3 finalTranslate = (1.0f - t.x) * vqs1.translate + t.x * vqs2.translate;

		Quaternion finalRotate = Quaternion::Slerp(vqs1.rotation, vqs2.rotation, t.y).Normalize();

		float finalScale;
		if (fabs(vqs1.scalar) > EPSILON)
		{
			finalScale = pow(vqs2.scalar / vqs1.scalar, t.z) * vqs1.scalar;
		}
		else
		{
			//finalScale = vqs2.scalar; // vqs2 scale as fallback.
			throw std::runtime_error("Division by zero or close to zero in VQS interpolation.");
		}

		return VQS(finalTranslate, finalRotate, finalScale);
	}

	glm::mat4 VQS::VQSToGLMMat4(VQS const& vqs)
	{

		glm::mat4 result = vqs.rotation.GetRotationMatrix();

		// Scale the rotation matrix
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				result[i][j] *= vqs.scalar;
			}
		}
		// Apply the translation
		result[3][0] = vqs.translate.x;
		result[3][1] = vqs.translate.y;
		result[3][2] = vqs.translate.z;

		return result;

		//  | Sx * R00  Sx * R01  Sx * R02  Tx |
		//	| Sy * R10  Sy * R11  Sy * R12  Ty |
		//	| Sz * R20  Sz * R21  Sz * R22  Tz |
		//	|   0      0      0      1 |

		//// translation matrix 
		//glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), vqs.translate);
		//// rotation matrix
		//glm::mat4 rotationMatrix = vqs.rotation.GetRotationMatrix();
		//// scaling matrix 
		//glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(vqs.scalar));
		//// Multiply the matrices together to get the final transformation
		//return translationMatrix * rotationMatrix * scalingMatrix;
	}

	VQS VQS::GLMMat4ToVQS(glm::mat4 const& mat)
	{
		VQS vqs;
		// Extract translation
		vqs.translate = glm::vec3(mat[3]);
		// Extract scale (assuming uniform scale)
		vqs.scalar = glm::length(glm::vec3(mat[0]));
		// Extract rotation
		glm::mat3 rotationMat = glm::mat3(mat);
		for (int i = 0; i < 3; i++)
		{
			rotationMat[i] = glm::normalize(rotationMat[i]);
		}
		glm::quat quat = glm::quat_cast(rotationMat);
		vqs.rotation = Quaternion::CreateFromGLMQuat(quat);
		return vqs;
	}

}

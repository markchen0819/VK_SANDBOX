#include "../pch.h"
#include "MathConstants.h"
#include "Quaternion.h"

namespace IHCEngine::Math
{
	Quaternion::Quaternion(float scalar, float x, float y, float z)
		:	scalar(scalar),
			imaginary(x, y, z)
	{}

	Quaternion::Quaternion(float scalar, glm::vec3 const& imaginary)
		:	scalar(scalar),
			imaginary(imaginary)
	{}

	Quaternion::Quaternion(glm::vec4 const& quaternion)
		:	scalar(quaternion.x),
			imaginary(quaternion.y, quaternion.z, quaternion.w)
		
	{
	}

	Quaternion::Quaternion(glm::vec3 const& rotationEuler)
	{
		// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		// q = cos(a/2) + v sin(a/2)
		//   = cos(a/2) + vx i sin(a/2) + vy j sin(a/2) + vz k sin(a/2) 

		// degree to radians
		glm::vec3 convertedEuler = rotationEuler * (PI / 180.0f);

		// Calculating the half-angles
		float cosX = glm::cos(convertedEuler.x * 0.5f);
		float cosY = glm::cos(convertedEuler.y * 0.5f);
		float cosZ = glm::cos(convertedEuler.z * 0.5f);
		float sinX = glm::sin(convertedEuler.x * 0.5f);
		float sinY = glm::sin(convertedEuler.y * 0.5f);
		float sinZ = glm::sin(convertedEuler.z * 0.5f);

		// Constructing the Quaternion (with individual axes)
		imaginary.x = cosZ * cosY * sinX - sinZ * sinY * cosX;
		imaginary.y = cosZ * sinY * cosX + sinZ * cosY * sinX;
		imaginary.z = sinZ * cosY * cosX - cosZ * sinY * sinX;
		scalar = cosZ * cosY * cosX + sinZ * sinY * sinX;
	}

	Quaternion::Quaternion(glm::vec3 const& unitAxis, float angle)
	{
		// https://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm
		float convertedAngle = angle * (PI / 180.0f);
		scalar = cosf(convertedAngle / 2.0f);
		imaginary = unitAxis * sinf(convertedAngle / 2.0f);
	}

	void Quaternion::SetImaginary(glm::vec3 const& imaginaryVector)
	{
		imaginary.x = imaginaryVector.x;
		imaginary.y = imaginaryVector.y;
		imaginary.z = imaginaryVector.z;
	}

	glm::vec3 Quaternion::GetImaginary() const
	{
		return imaginary;
	}

	Quaternion& Quaternion::operator=(Quaternion const& q)
	{
		scalar = q.scalar;
		imaginary = q.imaginary;
		return *this;
	}

	Quaternion Quaternion::operator*(float scalar)
	{
		return Quaternion(this->scalar * scalar, imaginary * scalar);
	}

	Quaternion Quaternion::operator/(float denom)
	{
		Quaternion quat(scalar / denom, imaginary / denom);
		return quat;
	}

	Quaternion Quaternion::operator*(Quaternion const& rhs) const
	{
		float resultScalarTerm;
		glm::vec3 resultImaginary;

		resultScalarTerm = scalar * rhs.scalar - (imaginary.x * rhs.imaginary.x + imaginary.y * rhs.imaginary.y + imaginary.z * rhs.imaginary.z);
		resultImaginary.x = imaginary.y * rhs.imaginary.z - imaginary.z * rhs.imaginary.y + scalar * rhs.imaginary.x + imaginary.x * rhs.scalar;
		resultImaginary.y = imaginary.z * rhs.imaginary.x - imaginary.x * rhs.imaginary.z + scalar * rhs.imaginary.y + imaginary.y * rhs.scalar;
		resultImaginary.z = imaginary.x * rhs.imaginary.y - imaginary.y * rhs.imaginary.x + scalar * rhs.imaginary.z + imaginary.z * rhs.scalar;

		return Quaternion(resultScalarTerm, resultImaginary);
	}

	Quaternion Quaternion::operator+(Quaternion const& rhs) const
	{
		return Quaternion(scalar + rhs.scalar, imaginary + rhs.imaginary);
	}

	Quaternion Quaternion::operator-(Quaternion const& rhs) const
	{
		glm::vec3 resultImaginary;
		resultImaginary.x = imaginary.x - rhs.imaginary.x;
		resultImaginary.y = imaginary.y - rhs.imaginary.y;
		resultImaginary.z = imaginary.z - rhs.imaginary.z;
		return Quaternion(scalar - rhs.scalar, resultImaginary);
	}

	Quaternion Quaternion::operator-() const
	{
		float x, y, z;
		x = -imaginary.x;
		y = -imaginary.y;
		z = -imaginary.z;
		return Quaternion(-scalar, x, y, z);
	}

	float Quaternion::Dot(Quaternion const& q1, Quaternion const& q2)
	{
		return
		(
			q1.scalar * q2.scalar
			+(q1.imaginary.x * q2.imaginary.x 
			+ q1.imaginary.y * q2.imaginary.y
			+ q1.imaginary.z * q2.imaginary.z) 
		);
	}

	float Quaternion::Length(Quaternion const& q)
	{
		return sqrt(LengthSquared(q));
	}

	float Quaternion::LengthSquared(Quaternion const& q)
	{
		float length = q.scalar * q.scalar
		+ q.imaginary.x * q.imaginary.x
		+ q.imaginary.y * q.imaginary.y
		+ q.imaginary.z * q.imaginary.z;
		return length;
	}

	Quaternion Quaternion::Conjugate(Quaternion const& q)
	{
		return Quaternion(q.scalar, -(q.imaginary));
	}

	Quaternion Quaternion::Inverse(Quaternion const& q)
	{
		return (Conjugate(q) / LengthSquared(q));
	}

	Quaternion Quaternion::Unit(Quaternion const& q)
	{
		return q * (1 / Length(q));
	}

	glm::mat4 Quaternion::GetMatrixOnLeftForMul() const
	{
		float x = imaginary.x;
		float y = imaginary.y;
		float z = imaginary.z;
		return glm::mat4
		(
			scalar, z, -y, -x,      
			-z, scalar, x, -y,   
			y, -x, scalar, -z,  
			x, y, z, scalar  
		);
	}

	glm::mat4 Quaternion::GetMatrixOnRightForMul() const
	{
		float x = imaginary.x;
		float y = imaginary.y;
		float z = imaginary.z;

		return glm::mat4
		(
			scalar, z, -y, x,     
			-z, scalar, x, y,    
			y, -x, scalar, z,   
			-x, -y, -z, scalar
		);

	}

	glm::mat4 Quaternion::GetRotationMatrix() const
	{
		float x = imaginary.x;
		float y = imaginary.y;
		float z = imaginary.z;

		return glm::mat4
		(
			1 - 2 * (y * y + z * z), 2 * (x * y + z * scalar), 2 * (x * z - y * scalar), 0.0f,   
			2 * (x * y - z * scalar), 1 - 2 * (x * x + z * z), 2 * (y * z + x * scalar), 0.0f,  
			2 * (x * z + y * scalar), 2 * (y * z - x * scalar), 1 - 2 * (x * x + y * y), 0.0f,  
			0.0f, 0.0f, 0.0f, 1.0f 
		);
	}

	glm::vec4 Quaternion::GetVectorForm() const
	{
		return glm::vec4(imaginary, scalar);
	}

	glm::quat Quaternion::ConvertToGLMQuat(const Quaternion& customQuat)
	{
		return glm::quat(customQuat.scalar, customQuat.imaginary.x, customQuat.imaginary.y, customQuat.imaginary.z);
	}

	Quaternion Quaternion::CreateFromGLMQuat(const glm::quat& glmQuat)
	{
		return Quaternion(glmQuat.w, glm::vec3(glmQuat.x, glmQuat.y, glmQuat.z));
	}

	Quaternion Quaternion::Slerp(Quaternion const& q1, Quaternion const& q2, float t)
	{
		Quaternion q3 = q2;
		float dot = Dot(q1, q2); // cos angle

		// Quaternions can represent the same rotation
		// with either a positive or negative sign
		// ensure we take shortest path 
		if (dot < 0)
		{
			dot = -dot;
			q3 = -q2;
		}

		if (dot < 0.95f)
		{
			float angle = acosf(dot);

			// Ensure that we don't divide by a very small value
			// ( q1 sin((1-t)theta)  +  q2 sin((t)theta)) / sin theta
			float denom = sinf(angle);
			if (fabs(denom) > EPSILON)
			{
				return (q1 * sinf(angle * (1 - t)) + q3 * sinf(angle * t)) / denom;
			}
		}

		// small angles use lerp to save computational costs.	
		return Lerp(q1, q3, t).Normalize();
	}

	Quaternion Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float t)
	{
		Quaternion q3 = (q1 * (1 - t) + q2 * t);
		return q3.Normalize();
	}

	Quaternion Quaternion::Normalize()
	{
		const float length = Length(*this);

		if (length > EPSILON)
		{
			*this = *this / length;
			return *this;
		}
		return *this;
	}
}


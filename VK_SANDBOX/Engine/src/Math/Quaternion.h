#pragma once

namespace IHCEngine::Math
{
	class Quaternion
	{
	public:
		// Default Unit Quaternion
		Quaternion(float scalar = 1.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f);
		Quaternion(float scalar, glm::vec3 const& imaginary);
		Quaternion(glm::vec4 const& quaternion); 

		Quaternion(glm::vec3 const& rotationEuler); 	// Euler angles
		Quaternion(glm::vec3 const& unitAxis, float angle); // Angle-axis 

		void SetScalar(float s) { scalar = s; }
		float GetScalar() const { return scalar; }
		void x(float x) { imaginary[0] = x; }
		float x() const { return imaginary[0]; }
		void y(float y) { imaginary[1] = y; }
		float y() const { return imaginary[1]; }
		void z(float z) { imaginary[2] = z; }
		float z() const { return imaginary[2]; }
		void SetImaginary(glm::vec3 const& imaginaryVector);
		glm::vec3 GetImaginary() const;

		// Operations
		Quaternion& operator=(Quaternion const& q);
		Quaternion operator*(float scalar);
		Quaternion operator/(float denom);
		Quaternion operator*(Quaternion const& rhs) const;
		Quaternion operator+(Quaternion const& rhs) const;
		Quaternion operator-(Quaternion const& rhs) const;
		Quaternion operator-() const;
		static inline float Dot(Quaternion const& q1, Quaternion const& q2);
		static inline float Length(Quaternion const& q);
		static inline float LengthSquared(Quaternion const& q);
		static inline Quaternion Conjugate(Quaternion const& q);
		static Quaternion Inverse(Quaternion const& q);
		static Quaternion Unit(Quaternion const& q);
		Quaternion Normalize();
		static Quaternion FromToRotation(const glm::vec3& from, const glm::vec3& to); // Creates a rotation which rotates from fromDirection to toDirection

		// matrix transformations 
		glm::mat4 GetMatrixOnLeftForMul() const;    //  matrix * rhs.vector = this * q
		glm::mat4 GetMatrixOnRightForMul() const;   //  rhs.vector.trans * matrix = q * vector.transpose
		glm::mat4 GetRotationMatrix() const;

		// matrix transformations 
		glm::vec4 GetVectorForm() const; //(x,y,z,w)


		// Between glm
		static glm::quat ConvertToGLMQuat(const Quaternion& customQuat);
		static Quaternion CreateFromGLMQuat(const glm::quat& glmQuat);

		// Interpolation
		static Quaternion Slerp(Quaternion const& q1, Quaternion const& q2, float t);
		static Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t);
		


	private:
		float scalar;
		glm::vec3 imaginary;


	};
}


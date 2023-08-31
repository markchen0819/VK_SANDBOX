#pragma once
#include "../Transform/Transform.h"

namespace IHCEngine::Graphics
{
	enum class CameraType 
	{
		PERSPECTIVE,
		ORTHOGRAPHIC
	};

	class Camera
	{
	public:

		Camera(CameraType type, float fov, float aspect, float near, float far, float orthoWidth,float orthoHeight);

		glm::mat4 GetProjectionMatrix();
		glm::mat4 GetViewMatrix();
		glm::mat4 GetInverseViewMatrix();

		float GetNearClippingPlane() { return nearPlane; };
		void SetNearClippingPlane(float d);// { nearPlane = d; };
		float GetFarClippingPlane() { return farPlane; };;
		void SetFarClippingPlane(float d);// { farPlane = d; };;
		float GetFOV() { return fov; }
		void SetFOV(float fieldOfView);// { fov = fieldOfView; };
		float GetAspectRatio() { return aspectRatio; }
		void SetAspectRatio(float width, float height);// { aspectRatio = width / height};

		void SetCameraType(CameraType type);

		// temporary
		void Update();
		GLFWwindow* window;



		//void SetViewDirection
		//(
		//	glm::vec3 position, 
		//	glm::vec3 direction, 
		//	glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f }
		//);
		//void SetViewTarget
		//(
		//	glm::vec3 position, glm::vec3 target, 
		//	glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f }
		//);
		//void SetViewYXZ(glm::vec3 position, glm::vec3 rotation); // EulerAngles

		//const glm::mat4& GetProjection() const { return projectionMatrix; }
		//const glm::mat4& GetView() const { return viewMatrix; }
		//const glm::mat4& GetInverseView() const { return inverseViewMatrix; }
		//const glm::vec3 GetPosition() const { return glm::vec3(inverseViewMatrix[3]); }

		// use position and rotation
		IHCEngine::Transform::Transform transform;


	private:

		CameraType type; // PERSPECTIVE or ORTHOGRAPHIC
		// Perspective
		float fov; // field of view in radians
		float aspectRatio; // aspect ratio of the viewport
		float nearPlane;
		float farPlane;
		// Orthographic 
		float orthoWidth; 
		float orthoHeight; 

		// Matrices
		glm::mat4 projectionMatrix{ 1.f }; // camera to screen (NDC)
		glm::mat4 viewMatrix{1.f};  // world to camera
		// transforms objects in the world space to the camera's view space.
		// represents the camera's position and orientation in the world.
		// moving and rotating everything in the world so that the camera is at the origin (0, 0, 0) and looking down the negative z-axi
		glm::mat4 inverseViewMatrix{1.f};
		// transforms from camera space back to world space.
		// common scenario (deferred rendering), wants the pixel pos in world

		// Flow:
		// Local Space -> (modelMatrix) 
		// ->  World Space -> (viewMatrix)
		// -> Camera Space -> (projectionMatrix) 
		// -> Clip Space -> (Perspective Division) -> 
		// -> Normalized Device Coordinates -> (Viewport Transformation)
		// -> Screen Space 
	};
}


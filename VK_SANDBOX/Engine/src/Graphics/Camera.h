#pragma once
#include "../Core/Scene/Components/Transform.h"

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
		void SetNearClippingPlane(float d){ isProjectionDirty = true; nearPlane = d; };
		float GetFarClippingPlane() { return farPlane; };
		void SetFarClippingPlane(float d){ isProjectionDirty = true; farPlane = d; };
		float GetFOV() { return fov; }
		void SetFOV(float fieldOfView){ isProjectionDirty = true; fov = fieldOfView; };
		float GetAspectRatio() { return aspectRatio; }
		void SetAspectRatio(float width, float height) { isProjectionDirty = true; aspectRatio = (width / height); };
		void SetAspectRatio(float ratio) { isProjectionDirty = true; aspectRatio = ratio; };
		void SetCameraType(CameraType type);

		IHCEngine::Component::Transform transform;

	private:

		CameraType type; // PERSPECTIVE or ORTHOGRAPHIC
		bool isProjectionDirty = true;

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

